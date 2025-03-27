#include <userver/components/component.hpp>
#include <userver/crypto/hash.hpp>
#include <userver/utils/uuid7.hpp>
#include <string_json_response.hpp>
#include <json_process.hpp>

#include "sign_in_up.hpp"

const std::string salt = "akksfmlkdsnvaeirnbaovnl;zlv13m12m4k1l24j0-34jg-34jgmglrkmg";

User Sign::GetUserParamsFromJsonBody(const userver::formats::json::Value& json_body) const {
    User user;
    try {
        user.password = userver::crypto::hash::Sha256(json_body["password"].As<std::string>() + salt);
        user.username = json_body["username"].As<std::string>();
    } catch (const userver::formats::json::Exception& ex) {
        throw userver::server::handlers::ClientError(userver::server::handlers::ExternalBody{ErrorResponse("Missed necessary field")});
    }
    return user;
}

std::string Sign::CreateToken() const { return userver::utils::generators::GenerateUuidV7(); }

void Sign::AddTokenToDB(int user_id, const std::string& token) const {
    auto expiry_date =
        userver::storages::postgres::TimePointTz{userver::utils::datetime::Now() + std::chrono::hours(24 * 7)};
    try {
        pg_cluster_->Execute(
            userver::storages::postgres::ClusterHostType::kMaster, kInsertToken, token, user_id, expiry_date
        );
    } catch (const userver::storages::postgres::Error& ex) {
        throw userver::server::handlers::InternalServerError(userver::server::handlers::ExternalBody{ErrorResponse("Internal error")});
    }
}

int SignIn::CheckUser(const User& user) const {
    int user_id;
    try {
        const auto& res =
            pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster, kGetPasswordAndId, user.username);
        if (res.IsEmpty() || res[0][0].As<std::string>() != user.password) {
            throw userver::server::handlers::ClientError(userver::server::handlers::ExternalBody{ErrorResponse("username or password are wrong")});
        }
        user_id = res[0][1].As<int>();
    } catch (const userver::storages::postgres::Error& ex) {
        throw userver::server::handlers::InternalServerError(userver::server::handlers::ExternalBody{ErrorResponse("Internal error")});
    }
    return user_id;
}

int SignUp::CreateUser(const User& user) const {
    int user_id;
    try {
        const auto& res = pg_cluster_->Execute(
            userver::storages::postgres::ClusterHostType::kMaster, kCreateUser, user.username, user.password
        );
        if (res.IsEmpty()) {
            throw userver::server::handlers::InternalServerError(userver::server::handlers::ExternalBody{ErrorResponse("Internal error")});
        }
        user_id = res[0][0].As<int>();
    } catch (const userver::storages::postgres::UniqueViolation& ex) {
        throw userver::server::handlers::ClientError{userver::server::handlers::ExternalBody{ErrorResponse("User already exists")}};
    } catch (const userver::storages::postgres::Error& ex) {
        throw userver::server::handlers::InternalServerError(userver::server::handlers::ExternalBody{ErrorResponse("Internal error")});
    }
    return user_id;
}

// TODO : добавить проверку существования токена перед выдачей нового
std::string
SignIn::HandleRequest(userver::server::http::HttpRequest& request, userver::server::request::RequestContext&) const {
    const auto& json_body = GetJsonBodyFromRequest(request);
    const auto& user = GetUserParamsFromJsonBody(json_body);

    int user_id = CheckUser(user);

    const std::string& token = CreateToken();
    AddTokenToDB(user_id, token);
    return TokenResponse(token);
}

std::string
SignUp::HandleRequest(userver::server::http::HttpRequest& request, userver::server::request::RequestContext&) const {
    const auto& json_body = GetJsonBodyFromRequest(request);
    const auto& user = GetUserParamsFromJsonBody(json_body);

    int user_id = CreateUser(user);

    const std::string& token = CreateToken();
    AddTokenToDB(user_id, token);
    return TokenResponse(token);
}