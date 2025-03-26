#include <userver/components/common_component_list.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>

class Sign : public userver::server::handlers::HttpHandlerBase {
public:
    Sign (const userver::components::ComponentConfig& config, const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context),
      pg_cluster_(context.FindComponent<userver::components::Postgres>("auth-database").GetCluster()) {}
    
protected:
    userver::formats::json::Value GetJsonBodyFromRequest(const userver::server::http::HttpRequest& request) const;
    std::string GetUsernameFromJsonBody(const userver::formats::json::Value& json_body) const;
    std::string GetPasswordFromJsonBody(const userver::formats::json::Value& json_body) const;
    std::string CreateToken() const;
    void AddTokenToDB(int user_id, const std::string& token) const;
    std::string StringTokenJson(const std::string& token) const;

    userver::storages::postgres::ClusterPtr pg_cluster_;
};

class SignIn final : public Sign {
public:
    static constexpr std::string_view kName = "handler-sign-in";
    
    using Sign::Sign;

    std::string HandleRequest(userver::server::http::HttpRequest& request, userver::server::request::RequestContext&) const override;
private:
    int CheckUser(const std::string& username, const std::string& password) const;
};

class SignUp final : public Sign {
public:
    static constexpr std::string_view kName = "handler-sign-up";

    using Sign::Sign;

    std::string HandleRequest(userver::server::http::HttpRequest& request, userver::server::request::RequestContext&) const override;
private:
    int CreateUser(const std::string& username, const std::string& password) const;
};