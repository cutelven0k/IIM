#include <userver/formats/json/value_builder.hpp>

#include "string_json_response.hpp"

std::string MessageResponse(const std::string& message) {
    userver::formats::json::ValueBuilder builder;
    builder["message"] = message;
    return userver::formats::json::ToString(builder.ExtractValue());
}

std::string ErrorResponse(const std::string& error) {
    userver::formats::json::ValueBuilder builder;
    builder["error"] = error;
    return userver::formats::json::ToString(builder.ExtractValue());
}