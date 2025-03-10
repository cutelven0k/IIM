/// [auth checker declaration]
#include "auth_bearer.hpp"
#include "user_info_cache.hpp"

#include <algorithm>

#include <userver/http/common_headers.hpp>

class AuthCheckerBearer final : public userver::server::handlers::auth::AuthCheckerBase {
public:
    using AuthCheckResult = userver::server::handlers::auth::AuthCheckResult;

    AuthCheckerBearer(const AuthCache& auth_cache, std::vector<userver::server::auth::UserScope> required_scopes)
        : auth_cache_(auth_cache), required_scopes_(required_scopes) {}

    [[nodiscard]] AuthCheckResult CheckAuth(
        const userver::server::http::HttpRequest& request,
        userver::server::request::RequestContext& request_context
    ) const override;

    [[nodiscard]] bool SupportsUserAuth() const noexcept override { return true; }

private:
    const AuthCache& auth_cache_;
    const std::vector<userver::server::auth::UserScope> required_scopes_;
};

AuthCheckerBearer::AuthCheckResult AuthCheckerBearer::CheckAuth(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext& request_context
) const {
    const auto& auth_value = request.GetHeader(userver::http::headers::kAuthorization);
    if (auth_value.empty()) {
        return AuthCheckResult{
            AuthCheckResult::Status::kTokenNotFound,
            {},
            "Empty 'Authorization' header",
            userver::server::handlers::HandlerErrorCode::kUnauthorized};
    }

    const auto bearer_sep_pos = auth_value.find(' ');
    if (bearer_sep_pos == std::string::npos || std::string_view{auth_value.data(), bearer_sep_pos} != "Bearer") {
        return AuthCheckResult{
            AuthCheckResult::Status::kTokenNotFound,
            {},
            "'Authorization' header should have 'Bearer some-token' format",
            userver::server::handlers::HandlerErrorCode::kUnauthorized};
    }

    const userver::server::auth::UserAuthInfo::Ticket token{auth_value.data() + bearer_sep_pos + 1};
    const auto cache_snapshot = auth_cache_.Get();

    auto it = cache_snapshot->find(token);
    if (it == cache_snapshot->end()) {
        return AuthCheckResult{AuthCheckResult::Status::kForbidden};
    }

    const UserDbInfo& info = it->second; 
    if (info.expiry_date < userver::storages::postgres::Now()) {
        return AuthCheckResult{
            AuthCheckResult::Status::kInvalidToken,
            {},
            "Token is expired",
            userver::server::handlers::HandlerErrorCode::kUnauthorized};
    }

    auto rolePriority = [](const std::string& role) -> int {
        if (role == "User") return 1;
        if (role == "Moderator") return 2;
        if (role == "Admin") return 3;
        return 0;
    };
    if (rolePriority(info.role) < rolePriority(required_scopes_[0].GetValue()))
        return 
        {AuthCheckResult::Status::kForbidden, {}, "No '" + required_scopes_[0].GetValue() + "' permission"};

    request_context.SetData("username", info.username);
    request_context.SetData("role", info.role);
    return {};
}

/// [auth checker factory definition]
userver::server::handlers::auth::AuthCheckerBasePtr CheckerFactory::
operator()(const userver::components::ComponentContext& context, const userver::server::handlers::auth::HandlerAuthConfig& auth_config, const userver::server::handlers::auth::AuthCheckerSettings&)
    const {
    auto scopes = auth_config["scopes"].As<userver::server::auth::UserScopes>({});
    const auto& auth_cache = context.FindComponent<AuthCache>();
    return std::make_shared<AuthCheckerBearer>(auth_cache, std::move(scopes));
}
/// [auth checker factory definition]
