#pragma once

#include <userver/components/common_component_list.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/components/component_context.hpp>

#include <userver/formats/json/value.hpp>
#include <userver/server/http/http_request.hpp>

struct ObjParams {
    double latitude;
    double longitude;
    std::string attachments;
    bool internal_amenities;
    bool external_amenities;
    float rating;
};

struct PermanentObjParams : ObjParams {
    std::string name;
};

struct TmpObjParams : ObjParams {
    std::string type;
};

const userver::storages::postgres::Query kAddPermanentObj{
    R"sql(
        INSERT INTO objs (latitude, longitude, name, internal_amenities, external_amenities, rating, attachments)
        VALUES ($1, $2, $3, $4, $5, $6, $7);
    )sql",
    userver::storages::postgres::Query::Name{"moderation_add_permanent_obj"}};

const userver::storages::postgres::Query kAddTmpObj{
    R"sql(
            INSERT INTO tmp_objs (latitude, longitude, type, internal_amenities, external_amenities, rating, attachments)
            VALUES ($1, $2, $3, $4, $5, $6, $7);
        )sql",
    userver::storages::postgres::Query::Name{"moderation_add_tmp_obj"}};

const userver::storages::postgres::Query kGetForModeration{
    R"sql(
            SELECT id, type, latitude, longitude, comment, attachments FROM moderation;
        )sql",
    userver::storages::postgres::Query::Name{"moderation_get_for_moderation"}};

const userver::storages::postgres::Query kDeleteObjFromModeration{
    R"sql(
            DELETE FROM moderation
            WHERE id = $1;
    )sql",
    userver::storages::postgres::Query::Name{"moderation_delete_from_moderation"}};

class GetForModeration final : public userver::server::handlers::HttpHandlerBase {
public:
    GetForModeration(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& context
    )
        : HttpHandlerBase(config, context),
            pg_cluster_(context.FindComponent<userver::components::Postgres>("auth-database").GetCluster()) {}

    static constexpr std::string_view kName = "handler-get-for-moderation";

    std::string HandleRequest(userver::server::http::HttpRequest& request, userver::server::request::RequestContext&) const override;
private:
    std::string GetModerationParamsFromDB() const;
    
    userver::storages::postgres::ClusterPtr pg_cluster_;
};

class DeleteFromModeration final : public userver::server::handlers::HttpHandlerBase {
public:
    DeleteFromModeration(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& context
    )
        : HttpHandlerBase(config, context),
            pg_cluster_(context.FindComponent<userver::components::Postgres>("auth-database").GetCluster()) {}

    static constexpr std::string_view kName = "handler-delete-from-moderation";

    std::string HandleRequest(userver::server::http::HttpRequest&, userver::server::request::RequestContext&) const override;
private:
    void DeleteModerationObjFromDB(int id) const;
    int GetModerationObjIdFromJsonBody(const userver::formats::json::Value& json_body) const;

    userver::storages::postgres::ClusterPtr pg_cluster_;
};

class AddObj final : public userver::server::handlers::HttpHandlerBase {
public:
    AddObj(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& context
    )
        : HttpHandlerBase(config, context),
          pg_cluster_(context.FindComponent<userver::components::Postgres>("auth-database").GetCluster()) {}

    static constexpr std::string_view kName = "handler-add-obj";

    std::string HandleRequest(userver::server::http::HttpRequest& request, userver::server::request::RequestContext&) const override;

private:
    PermanentObjParams GetPermanentObjParamsFromJsonBody(const userver::formats::json::Value& json_body) const;
    TmpObjParams GetTmpObjParamsFromJsonBody(const userver::formats::json::Value& json_body) const;


    std::string AddPermanentObj(const userver::formats::json::Value& json_body) const;
    std::string AddTmpObj(const userver::formats::json::Value& json_body) const;


    void AddPermanentObjToDB(const PermanentObjParams& permanet) const;
    void AddTmpObjToDB(const TmpObjParams& permanet) const;

    userver::storages::postgres::ClusterPtr pg_cluster_;
};