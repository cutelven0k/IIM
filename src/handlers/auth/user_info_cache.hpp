#pragma once

#include <userver/cache/base_postgres_cache.hpp>
#include <userver/crypto/algorithm.hpp>
#include <userver/server/auth/user_auth_info.hpp>
#include <userver/storages/postgres/io/array_types.hpp>

struct UserDbInfo {
    userver::server::auth::UserAuthInfo::Ticket token;
    std::int64_t user_id;
    std::string role;
    std::string username;
    userver::storages::postgres::TimePointTz expiry_date;
};

struct AuthCachePolicy {
    static constexpr std::string_view kName = "auth-pg-cache";

    using ValueType = UserDbInfo;

    static constexpr auto kKeyMember = &UserDbInfo::token;

    static constexpr const char* kQuery = R"sql(
        SELECT 
            t.token, 
            u.id AS user_id, 
            u.role, 
            u.username, 
            t.expiry_date
        FROM tokens t
        JOIN users u ON t.user_id = u.id;
    )sql";

    static constexpr const char* kUpdatedField = "updated";

    using UpdatedFieldType = userver::storages::postgres::TimePointTz;

    // Using crypto::algorithm::StringsEqualConstTimeComparator to avoid timing
    // attack at find(token).
    using CacheContainer = std::unordered_map<
        userver::server::auth::UserAuthInfo::Ticket,
        UserDbInfo,
        std::hash<userver::server::auth::UserAuthInfo::Ticket>,
        userver::crypto::algorithm::StringsEqualConstTimeComparator>;
};

using AuthCache = userver::components::PostgreCache<AuthCachePolicy>;
