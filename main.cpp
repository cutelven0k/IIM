#include "auth_bearer.hpp"
#include "user_info_cache.hpp"

#include <userver/clients/dns/component.hpp>
#include <userver/testsuite/testsuite_support.hpp>

#include <userver/components/minimal_server_component_list.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/utils/daemon_run.hpp>

#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>

// #include <generated/static_config.yaml.hpp>

class UserHandler final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-user";

    using HttpHandlerBase::HttpHandlerBase;

    std::string HandleRequest(userver::server::http::HttpRequest& request, userver::server::request::RequestContext& ctx) const override {
        request.GetHttpResponse().SetContentType(userver::http::content_type::kTextPlain);
        return "Name: " + ctx.GetData<std::string>("username") + "\n" + "Role: " + ctx.GetData<std::string>("role") + "\n";
    }
};

class AdminHandler final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-admin";

    using HttpHandlerBase::HttpHandlerBase;

    std::string HandleRequest(userver::server::http::HttpRequest& request, userver::server::request::RequestContext& ctx) const override {
        request.GetHttpResponse().SetContentType(userver::http::content_type::kTextPlain);
        return "Name: " + ctx.GetData<std::string>("username") + "\n" + "Role: " + ctx.GetData<std::string>("role") + "\n";
    }
};
class ModeratorHandler final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-moderator";

    using HttpHandlerBase::HttpHandlerBase;

    std::string HandleRequest(userver::server::http::HttpRequest& request, userver::server::request::RequestContext& ctx) const override {
        request.GetHttpResponse().SetContentType(userver::http::content_type::kTextPlain);
        return "Name: " + ctx.GetData<std::string>("username") + "\n" + "Role: " + ctx.GetData<std::string>("role") + "\n";
    }
};

/// [auth checker registration]
int main(int charc, const char *const charv[]) {
    userver::server::handlers::auth::RegisterAuthCheckerFactory("bearer", std::make_unique<CheckerFactory>());
    /// [auth checker registration]

    /// [main]
    const auto component_list = userver::components::MinimalServerComponentList()
                                    .Append<AuthCache>()
                                    .Append<userver::components::Postgres>("auth-database")
                                    .Append<UserHandler>()
                                    .Append<AdminHandler>()
                                    .Append<ModeratorHandler>()
                                    .Append<userver::components::TestsuiteSupport>()
                                    .Append<userver::clients::dns::Component>();
    return userver::utils::DaemonMain(charc, charv, component_list);
    /// [main]
}
/// [Postgres service sample - main]