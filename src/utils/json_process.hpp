#pragma once

#include <userver/formats/json/value.hpp>
#include <userver/server/http/http_request.hpp>

userver::formats::json::Value GetJsonBodyFromRequest(const userver::server::http::HttpRequest& request);