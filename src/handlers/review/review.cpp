#include <json_process.hpp>
#include <string_json_response.hpp>
#include "review.hpp"

ReviewParams AddReview::GetReviewParamsFromJsonBody(const userver::formats::json::Value& json_body) const {
    ReviewParams review_params;
    try {
        review_params.user_id = json_body["user_id"].As<int>();
        review_params.obj_id = json_body["obj_id"].As<int>();
        review_params.rating = json_body["rating"].As<int>();
        review_params.review = json_body["review"].As<std::string>();
        review_params.attachments = json_body["attachments"].As<std::string>();

    } catch (const userver::formats::json::Exception& ex) {
        throw userver::server::handlers::ClientError(userver::server::handlers::ExternalBody{ErrorResponse("Missed necessary field")});
    }
    return review_params;
}

int ChangeReview::GetReviewIdFromJsonBody(const userver::formats::json::Value& json_body) const {
    int id;
    try {
        id = json_body["review_id"].As<int>();
    } catch (const userver::formats::json::Exception& ex) {
        throw userver::server::handlers::ClientError(userver::server::handlers::ExternalBody{ErrorResponse("Missed necessary field")});
    }
    return id;
}


void AddReview::AddReviewToDB(const ReviewParams& review_params) const {
    try {
        pg_cluster_->Execute(
            userver::storages::postgres::ClusterHostType::kMaster,
            kAddReview,
            review_params.user_id,
            review_params.obj_id,
            review_params.rating,
            review_params.review,
            review_params.attachments
        );
    } catch (const userver::storages::postgres::Error& ex) {
        throw userver::server::handlers::InternalServerError(userver::server::handlers::ExternalBody{ErrorResponse("Internal error")});
    }
}

int ChangeReview::GetUserIdFromDB(int review_id) const {
    int id;
    try {
        const auto& res = pg_cluster_->Execute(
            userver::storages::postgres::ClusterHostType::kMaster,
            kGetUserId,
            review_id
        );
        if (res.IsEmpty()) {
            throw userver::server::handlers::ClientError(userver::server::handlers::ExternalBody{ErrorResponse("No access for this review")});
        }
        id = res[0][0].As<int>();
    } catch (const userver::storages::postgres::Error& ex) {
        throw userver::server::handlers::InternalServerError(userver::server::handlers::ExternalBody{ErrorResponse("Internal error")});
    } catch (const userver::formats::json::Exception& ex) {
        throw userver::server::handlers::ClientError(userver::server::handlers::ExternalBody{ErrorResponse("Missed necessary field")});
    }
    return id;
}

void DeleteReview::DeleteReviewFromDB(int review_id) const {
    try {
        pg_cluster_->Execute(
            userver::storages::postgres::ClusterHostType::kMaster,
            kDeleteReview,
            review_id
        );
    } catch (const userver::storages::postgres::Error& ex) {
        throw userver::server::handlers::InternalServerError(userver::server::handlers::ExternalBody{ErrorResponse("Internal error")});
    }
}

void EditReview::InsertUpdatedReviewParamsToDB(const EditableReviewParams& editable_review_params, int reveiw_id) const {
    try {
        pg_cluster_->Execute(
            userver::storages::postgres::ClusterHostType::kMaster,
            kUpdateReview,
            editable_review_params.rating,
            editable_review_params.review,
            editable_review_params.attachments,
            reveiw_id
        );
    } catch (const userver::storages::postgres::Error& ex) {
        throw userver::server::handlers::InternalServerError(userver::server::handlers::ExternalBody{ErrorResponse("Internal error")});
    }
}

EditableReviewParams EditReview::GetEditedReviewParamsFromJsonBody(const userver::formats::json::Value& json_body) const {
    EditableReviewParams editable_review_params;
    
    try {
        if (json_body.HasMember("rating"))
            editable_review_params.rating = json_body["rating"].As<int>();
        if (json_body.HasMember("review"))
            editable_review_params.review = json_body["review"].As<std::string>();
        if (json_body.HasMember("attachments"))
            editable_review_params.review = json_body["attachments"].As<std::string>();
    } catch (const userver::formats::json::Exception& ex) {
        throw userver::server::handlers::ClientError(userver::server::handlers::ExternalBody{ErrorResponse("Wrong type of fields")});
    }

    return editable_review_params;    
}



std::string AddReview::HandleRequest(userver::server::http::HttpRequest& request, userver::server::request::RequestContext&) const {
    const auto& json_body = GetJsonBodyFromRequest(request);
    const auto& review_params = GetReviewParamsFromJsonBody(json_body);
    AddReviewToDB(review_params);
    return MessageResponse("Review added");
}

std::string EditReview::HandleRequest(userver::server::http::HttpRequest& request, userver::server::request::RequestContext& request_context) const {
    const auto& json_body = GetJsonBodyFromRequest(request);
    int review_id = GetReviewIdFromJsonBody(json_body);
    int user_id_from_context = request_context.GetData<int>("user_id");
    int user_id_from_db = GetUserIdFromDB(review_id);
    if (user_id_from_context != user_id_from_db) {
        throw userver::server::handlers::ClientError(userver::server::handlers::ExternalBody{ErrorResponse("No access for this review")});
    }

    const auto& editable_review_params = GetEditedReviewParamsFromJsonBody(json_body);
    InsertUpdatedReviewParamsToDB(editable_review_params, review_id);
    return MessageResponse("Review successfully updated");
}

std::string DeleteReview::HandleRequest(userver::server::http::HttpRequest& request, userver::server::request::RequestContext& request_context) const {
    const auto& json_body = GetJsonBodyFromRequest(request);
    int review_id = GetReviewIdFromJsonBody(json_body);
    int user_id_from_context = request_context.GetData<int>("user_id");

    int user_id_from_db = GetUserIdFromDB(review_id);
    if (user_id_from_context != user_id_from_db) {
        throw userver::server::handlers::ClientError(userver::server::handlers::ExternalBody{ErrorResponse("No access for this review")});
    }
    DeleteReviewFromDB(review_id);
    return MessageResponse("Review successfully deleted");
}
