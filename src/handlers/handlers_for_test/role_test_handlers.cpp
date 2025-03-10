#include "role_test_handlers.hpp"

std::string UserHandler::HandleRequest(
    userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext& ctx
) const {
    request.GetHttpResponse().SetContentType(userver::http::content_type::kTextPlain);
    return "Name: " + ctx.GetData<std::string>("username") + "\n" + "Role: " + ctx.GetData<std::string>("role") + "\n";
}

std::string AdminHandler::HandleRequest(
    userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext& ctx
) const {
    request.GetHttpResponse().SetContentType(userver::http::content_type::kTextPlain);
    return "Name: " + ctx.GetData<std::string>("username") + "\n" + "Role: " + ctx.GetData<std::string>("role") + "\n";
}

std::string ModeratorHandler::HandleRequest(
    userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext& ctx
) const {
    request.GetHttpResponse().SetContentType(userver::http::content_type::kTextPlain);
    return "Name: " + ctx.GetData<std::string>("username") + "\n" + "Role: " + ctx.GetData<std::string>("role") + "\n";
}
