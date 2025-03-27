#include <userver/server/handlers/exceptions.hpp>
#include <string_json_response.hpp>
#include <json_process.hpp>

#include "moderation.hpp"

int DeleteFromModeration::GetModerationObjIdFromJsonBody(const userver::formats::json::Value& json_body) const {
    int id;
    try {
        id = json_body["id"].As<int>();
    } catch (const userver::formats::json::Exception& ex) {
        throw userver::server::handlers::ClientError(userver::server::handlers::ExternalBody{ErrorResponse("Missed necessary field")});
    }
    return id;
}


PermanentObjParams AddObj::GetPermanentObjParamsFromJsonBody(const userver::formats::json::Value& json_body) const {
    PermanentObjParams permanent_obj_params;
    try {
        permanent_obj_params.latitude = json_body["latitude"].As<double>();
        permanent_obj_params.longitude = json_body["longitude"].As<double>();
        permanent_obj_params.name = json_body["name"].As<std::string>();
        permanent_obj_params.internal_amenities = json_body["internal_amenities"].As<bool>();
        permanent_obj_params.external_amenities = json_body["external_amenities"].As<bool>();
        permanent_obj_params.rating = json_body["rating"].As<float>();
        permanent_obj_params.attachments = json_body["attachments"].As<std::string>();
    } catch (const userver::formats::json::Exception& ex) {
        throw userver::server::handlers::ClientError(userver::server::handlers::ExternalBody{ErrorResponse("Missed necessary field")});
    }
    return permanent_obj_params;
}

TmpObjParams AddObj::GetTmpObjParamsFromJsonBody(const userver::formats::json::Value& json_body) const {
    TmpObjParams tmp_obj_params;
    try {
        tmp_obj_params.latitude = json_body["latitude"].As<double>();
        tmp_obj_params.longitude = json_body["longitude"].As<double>();
        tmp_obj_params.type = json_body["type"].As<std::string>();
        tmp_obj_params.internal_amenities = json_body["internal_amenities"].As<bool>();
        tmp_obj_params.external_amenities = json_body["external_amenities"].As<bool>();
        tmp_obj_params.rating = json_body["rating"].As<float>();
        tmp_obj_params.attachments = json_body["attachments"].As<std::string>();
    } catch (const userver::formats::json::Exception& ex) {
        throw userver::server::handlers::ClientError(userver::server::handlers::ExternalBody{ErrorResponse("Missed necessary field")});
    }
    return tmp_obj_params;
}

std::string GetForModeration::GetModerationParamsFromDB() const {
    userver::formats::json::ValueBuilder json_array(userver::formats::json::Type::kArray);
    try {
        const auto& res = pg_cluster_->Execute(
            userver::storages::postgres::ClusterHostType::kMaster,
            kGetForModeration
        );
        if (res.IsEmpty()) {
            LOG_ERROR() << "Empty res\n";
            throw userver::server::handlers::InternalServerError(userver::server::handlers::ExternalBody{ErrorResponse("Internal error")});
        }
        for (const auto& row : res) {
            userver::formats::json::ValueBuilder obj;
            obj["id"] = row[0].As<int>();
            obj["type"] = row[1].As<std::string>();
            obj["latitude"] = row[2].As<double>();
            obj["longitude"] = row[3].As<double>();
            obj["comment"] = row[4].As<std::string>();
            obj["attachments"] = row[5].As<std::string>();
            json_array.PushBack(obj.ExtractValue());
        }
    } catch (const userver::storages::postgres::Error& ex) {
        LOG_ERROR() << ex;
        throw userver::server::handlers::InternalServerError(userver::server::handlers::ExternalBody{ErrorResponse("Internal error")});
    } catch (const userver::formats::json::Exception& ex) {
        throw userver::server::handlers::ClientError(userver::server::handlers::ExternalBody{ErrorResponse("Missed necessary field")});
    }
    return userver::formats::json::ToString(json_array.ExtractValue());
}

void DeleteFromModeration::DeleteModerationObjFromDB(int id) const {
    try {
        pg_cluster_->Execute(
            userver::storages::postgres::ClusterHostType::kMaster,
            kDeleteObjFromModeration,
            id
        );
    } catch (const userver::storages::postgres::Error& ex) {
        LOG_ERROR() << ex;
        throw userver::server::handlers::InternalServerError(userver::server::handlers::ExternalBody{ErrorResponse("Internal error")});
    }
}


void AddObj::AddPermanentObjToDB(const PermanentObjParams& permanent_obj_params) const {
    try {
        pg_cluster_->Execute(
            userver::storages::postgres::ClusterHostType::kMaster,
            kAddPermanentObj,
            permanent_obj_params.latitude,
            permanent_obj_params.longitude,
            permanent_obj_params.name,
            permanent_obj_params.internal_amenities,
            permanent_obj_params.external_amenities,
            permanent_obj_params.rating,
            permanent_obj_params.attachments
        );
    } catch (const userver::storages::postgres::Error& ex) {
        throw userver::server::handlers::InternalServerError(userver::server::handlers::ExternalBody{ErrorResponse("Internal error")});
    }
}

void AddObj::AddTmpObjToDB(const TmpObjParams& tmp_obj_params) const {
    try {
        pg_cluster_->Execute(
            userver::storages::postgres::ClusterHostType::kMaster,
            kAddTmpObj,
            tmp_obj_params.latitude,
            tmp_obj_params.longitude,
            tmp_obj_params.type,
            tmp_obj_params.internal_amenities,
            tmp_obj_params.external_amenities,
            tmp_obj_params.rating,
            tmp_obj_params.attachments
        );
    } catch (const userver::storages::postgres::Error& ex) {
        throw userver::server::handlers::InternalServerError(userver::server::handlers::ExternalBody{ErrorResponse("Internal error")});
    }
}

std::string
GetForModeration::HandleRequest(userver::server::http::HttpRequest&, userver::server::request::RequestContext&) const {
    const auto& moderation_params = GetModerationParamsFromDB();
    return moderation_params;
}

std::string
DeleteFromModeration::HandleRequest(userver::server::http::HttpRequest& request, userver::server::request::RequestContext&) const {
    const auto& json_body = GetJsonBodyFromRequest(request);
    int id = GetModerationObjIdFromJsonBody(json_body);
    DeleteModerationObjFromDB(id);
    return MessageResponse("Moderation obj deleted");
}

std::string
AddObj::HandleRequest(userver::server::http::HttpRequest& request, userver::server::request::RequestContext&) const {
    const auto& json_body = GetJsonBodyFromRequest(request);
    std::string type;
    int id;
    try {
        type = json_body["type"].As<std::string>();
        id = json_body["id"].As<int>();
    } catch (const userver::formats::json::Exception& ex) {
        throw userver::server::handlers::ClientError(userver::server::handlers::ExternalBody{ErrorResponse("Missed necessary field")});
    }

    std::string response;
    if (type == "Temp") {
        response = AddTmpObj(json_body);
    } else if (type == "Permanent") {
        response = AddPermanentObj(json_body);
    } else {
        throw userver::server::handlers::ClientError(userver::server::handlers::ExternalBody{ErrorResponse("Non-existent type")});
    }

    try {
        pg_cluster_->Execute(
            userver::storages::postgres::ClusterHostType::kMaster,
            kDeleteObjFromModeration,
            id
        );
    } catch (const userver::storages::postgres::Error& ex) {
        throw userver::server::handlers::InternalServerError(userver::server::handlers::ExternalBody{ErrorResponse("Internal error")});
    }

    return MessageResponse(response);
}

std::string AddObj::AddPermanentObj(const userver::formats::json::Value& json_body) const {
    const auto& permament_obj_params = GetPermanentObjParamsFromJsonBody(json_body);
    AddPermanentObjToDB(permament_obj_params);
    return "permanent obj successfully added to db";
}

std::string AddObj::AddTmpObj(const userver::formats::json::Value& json_body) const {
    const auto& tmp_obj_params = GetTmpObjParamsFromJsonBody(json_body);
    AddTmpObjToDB(tmp_obj_params);
    return "tmp obj successfully added to db";
}
