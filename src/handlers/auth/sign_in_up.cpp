#include <userver/components/component.hpp>
#include <userver/crypto/hash.hpp>
#include <userver/utils/uuid7.hpp>

#include "sign_in_up.hpp"

const std::string& salt = "akksfmlkdsnvaeirnbaovnl;zlv13m12m4k1l24j0-34jg-34jgmglrkmg";

// Queries
const userver::storages::postgres::Query kGetPasswordAndId{
    R"sql(
        SELECT password, id FROM users 
        WHERE username = $1;
    )sql",
    userver::storages::postgres::Query::Name{"sig_in_up_get_password_and_id"}};

const userver::storages::postgres::Query kInsertToken{
    R"sql(
        INSERT INTO tokens (token, user_id, expiry_date) 
        VALUES ($1, $2, $3);
    )sql",
    userver::storages::postgres::Query::Name{"sign_in_up_insert_token"}};

const userver::storages::postgres::Query kCreateUser{
    R"sql(
        INSERT INTO users (username, password, role)
        VALUES ($1, $2, 'User')
        RETURNING id;
    )sql",
    userver::storages::postgres::Query::Name{"sign_in_up_create_user"}};
//

userver::formats::json::Value Sign::GetJsonBodyFromRequest(const userver::server::http::HttpRequest& request) const {
    const auto& body = request.RequestBody();
    if (body.empty()) {
        throw userver::server::handlers::ClientError(userver::server::handlers::ExternalBody{"Empty body"});
    }
    userver::formats::json::Value json_body;
    try {
        json_body = userver::formats::json::FromString(body);
    } catch (const userver::formats::json::Exception& ex) {
        throw userver::server::handlers::ClientError(userver::server::handlers::ExternalBody{"Wrong json structure"});
    }
    return json_body;
}

std::string Sign::GetUsernameFromJsonBody(const userver::formats::json::Value& json_body) const {
    std::string username;
    try {
        username = json_body["username"].As<std::string>();
    } catch (const userver::formats::json::Exception& ex) {
        throw userver::server::handlers::ClientError(userver::server::handlers::ExternalBody{"Missed username field"});
    }
    return username;
}

std::string Sign::GetPasswordFromJsonBody(const userver::formats::json::Value& json_body) const {
    std::string password;
    try {
        password = userver::crypto::hash::Sha256(json_body["password"].As<std::string>() + salt);
    } catch (const userver::formats::json::Exception& ex) {
        throw userver::server::handlers::ClientError(userver::server::handlers::ExternalBody{"Missed password field"});
    }
    return password;
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
        throw userver::server::handlers::InternalServerError(userver::server::handlers::ExternalBody{"Internal error"});
    }
}

std::string Sign::StringTokenJson(const std::string& token) const {
    userver::formats::json::ValueBuilder builder;
    builder["token"] = token;
    return userver::formats::json::ToString(builder.ExtractValue());
}

int SignIn::CheckUser(const std::string& username, const std::string& password) const {
    int user_id;
    try {
        const auto& res =
            pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster, kGetPasswordAndId, username);
        if (res.IsEmpty() || res[0][0].As<std::string>() != password) {
            throw userver::server::handlers::ClientError(userver::server::handlers::ExternalBody{"username or password are wrong"});
        }
        user_id = res[0][1].As<int>();
    } catch (const userver::storages::postgres::Error& ex) {
        throw userver::server::handlers::InternalServerError(userver::server::handlers::ExternalBody{"Internal error"});
    }
    return user_id;
}

// TODO : добавить проверку существования токена перед выдачей нового
std::string
SignIn::HandleRequest(userver::server::http::HttpRequest& request, userver::server::request::RequestContext&) const {
    const auto& json_body = GetJsonBodyFromRequest(request);
    const std::string& username = GetUsernameFromJsonBody(json_body);
    const std::string& password = GetPasswordFromJsonBody(json_body);

    int user_id = CheckUser(username, password);

    const std::string& token = CreateToken();
    AddTokenToDB(user_id, token);
    return StringTokenJson(token);
}

int SignUp::CreateUser(const std::string& username, const std::string& password) const {
    int user_id;
    try {
        const auto& res = pg_cluster_->Execute(
            userver::storages::postgres::ClusterHostType::kMaster, kCreateUser, username, password
        );
        if (res.IsEmpty()) {
            throw userver::server::handlers::InternalServerError(userver::server::handlers::ExternalBody{"Internal error"});
        }
        user_id = res[0][0].As<int>();
    } catch (const userver::storages::postgres::UniqueViolation& ex) {
        throw userver::server::handlers::ClientError{userver::server::handlers::ExternalBody{"User already exists"}};
    } catch (const userver::storages::postgres::Error& ex) {
        throw userver::server::handlers::InternalServerError(userver::server::handlers::ExternalBody{"Internal error"});
    }
    return user_id;
}

std::string
SignUp::HandleRequest(userver::server::http::HttpRequest& request, userver::server::request::RequestContext&) const {
    const auto& json_body = GetJsonBodyFromRequest(request);
    const std::string& username = GetUsernameFromJsonBody(json_body);
    const std::string& password = GetPasswordFromJsonBody(json_body);

    int user_id = CreateUser(username, password);

    const std::string& token = CreateToken();
    AddTokenToDB(user_id, token);
    return StringTokenJson(token);
}