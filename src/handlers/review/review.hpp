# pragma once

#include <userver/components/common_component_list.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/components/component_context.hpp>

struct EditableReviewParams {
    std::optional<int> rating;
    std::optional<std::string> review;
    std::optional<std::string> attachments;
};

struct ReviewParams {
    int rating;
    std::string review;
    std::string attachments;
    int user_id;
    int obj_id;
};

const userver::storages::postgres::Query kAddReview{
    R"sql(
        INSERT INTO reviews (user_id, obj_id, rating, review, attachments)
        VALUES ($1, $2, $3, $4, $5);
    )sql",
    userver::storages::postgres::Query::Name{"review_add_review"}};

const userver::storages::postgres::Query kGetUserId{
    R"sql(
        SELECT user_id FROM reviews
        WHERE id = $1;
    )sql",
    userver::storages::postgres::Query::Name{"review_get_user_id"}};

const userver::storages::postgres::Query kDeleteReview{
    R"sql(
        DELETE FROM reviews
        WHERE id = $1
    )sql",
    userver::storages::postgres::Query::Name{"review_delete_review"}};

const userver::storages::postgres::Query kUpdateReview{
    R"sql(
        UPDATE reviews
        SET
            rating = COALESCE($1, rating),
            review = COALESCE($2, review),
            attachments = COALESCE($3, attachments)
        WHERE id = $4;
    )sql",
    userver::storages::postgres::Query::Name{"review_update_review"}};


class AddReview final : public userver::server::handlers::HttpHandlerBase {
public:
    AddReview(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& context
    )
        : HttpHandlerBase(config, context),
            pg_cluster_(context.FindComponent<userver::components::Postgres>("auth-database").GetCluster()) {}

    static constexpr std::string_view kName = "handler-add-review";

    std::string HandleRequest(userver::server::http::HttpRequest& request, userver::server::request::RequestContext&) const override;
private:
    ReviewParams GetReviewParamsFromJsonBody(const userver::formats::json::Value& json_body) const;
    void AddReviewToDB(const ReviewParams& review_params) const;
    
    userver::storages::postgres::ClusterPtr pg_cluster_;
};

class ChangeReview : public userver::server::handlers::HttpHandlerBase {
public:
    ChangeReview(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& context
    )
        : HttpHandlerBase(config, context),
            pg_cluster_(context.FindComponent<userver::components::Postgres>("auth-database").GetCluster()) {}
protected:
    int GetReviewIdFromJsonBody(const userver::formats::json::Value& json_body) const;
    int GetUserIdFromDB(int review_id) const;

    userver::storages::postgres::ClusterPtr pg_cluster_;
};

class EditReview final : public ChangeReview {
public:
    using ChangeReview::ChangeReview;

    static constexpr std::string_view kName = "handler-edit-review";

    std::string HandleRequest(userver::server::http::HttpRequest& request, userver::server::request::RequestContext& request_context) const override;
private:
    EditableReviewParams GetEditedReviewParamsFromJsonBody(const userver::formats::json::Value& json_body) const;
    void InsertUpdatedReviewParamsToDB(const EditableReviewParams& editable_review_params, int reveiw_id) const;
};

class DeleteReview final : public ChangeReview{
public:
    using ChangeReview::ChangeReview;

    static constexpr std::string_view kName = "handler-delete-review";

    std::string HandleRequest(userver::server::http::HttpRequest& request, userver::server::request::RequestContext& request_context) const override;
private:
    void DeleteReviewFromDB(int review_id) const;
};