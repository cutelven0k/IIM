#include "json_process.hpp"

#include <userver/server/handlers/exceptions.hpp>

#include <string_json_response.hpp>

userver::formats::json::Value GetJsonBodyFromRequest(const userver::server::http::HttpRequest& request) {
    const auto& body = request.RequestBody();
    if (body.empty()) {
        throw userver::server::handlers::ClientError(userver::server::handlers::ExternalBody{ErrorResponse("Empty body")});
    }
    userver::formats::json::Value json_body;
    try {
        json_body = userver::formats::json::FromString(body);
    } catch (const userver::formats::json::Exception& ex) {
        throw userver::server::handlers::ClientError(userver::server::handlers::ExternalBody{ErrorResponse("Wrong json structure")});
    }
    return json_body;
}