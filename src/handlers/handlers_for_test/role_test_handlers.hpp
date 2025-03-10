#include <userver/components/common_component_list.hpp>
#include <userver/server/handlers/http_handler_base.hpp>

class UserHandler final : public userver::server::handlers::HttpHandlerBase {
    public:
        static constexpr std::string_view kName = "handler-user";
    
        using HttpHandlerBase::HttpHandlerBase;
    
        std::string HandleRequest(
            userver::server::http::HttpRequest& request,
            userver::server::request::RequestContext& ctx
        ) const override;
    };
    
    class AdminHandler final : public userver::server::handlers::HttpHandlerBase {
    public:
        static constexpr std::string_view kName = "handler-admin";
    
        using HttpHandlerBase::HttpHandlerBase;
    
        std::string HandleRequest(
            userver::server::http::HttpRequest& request,
            userver::server::request::RequestContext& ctx
        ) const override;
    };
    class ModeratorHandler final : public userver::server::handlers::HttpHandlerBase {
    public:
        static constexpr std::string_view kName = "handler-moderator";
    
        using HttpHandlerBase::HttpHandlerBase;
    
        std::string HandleRequest(
            userver::server::http::HttpRequest& request,
            userver::server::request::RequestContext& ctx
        ) const override;
    };
    