#pragma once

#include <userver/components/common_component_list.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>

extern const std::string salt;

struct User {
    std::string username;
    std::string password;
};

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

class Sign : public userver::server::handlers::HttpHandlerBase {
public:
    Sign (const userver::components::ComponentConfig& config, const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context),
      pg_cluster_(context.FindComponent<userver::components::Postgres>("auth-database").GetCluster()) {}
    
protected:
    User GetUserParamsFromJsonBody(const userver::formats::json::Value& json_body) const;

    std::string CreateToken() const;
    
    void AddTokenToDB(int user_id, const std::string& token) const;

    userver::storages::postgres::ClusterPtr pg_cluster_;
};

class SignIn final : public Sign {
public:
    static constexpr std::string_view kName = "handler-sign-in";
    
    using Sign::Sign;

    std::string HandleRequest(userver::server::http::HttpRequest& request, userver::server::request::RequestContext&) const override;
private:
    int CheckUser(const User& user) const;
};

class SignUp final : public Sign {
public:
    static constexpr std::string_view kName = "handler-sign-up";

    using Sign::Sign;

    std::string HandleRequest(userver::server::http::HttpRequest& request, userver::server::request::RequestContext&) const override;
private:
    int CreateUser(const User& user) const;
};