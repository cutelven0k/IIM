#include <auth_bearer.hpp>
#include <user_info_cache.hpp>
#include <moderation.hpp>

#include <userver/clients/dns/component.hpp>
#include <userver/testsuite/testsuite_support.hpp>

#include <userver/components/minimal_server_component_list.hpp>
#include <userver/utils/daemon_run.hpp>

#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>

// #include <generated/static_config.yaml.hpp>

int main(int charc, const char* const charv[]) {
    userver::server::handlers::auth::RegisterAuthCheckerFactory("bearer", std::make_unique<CheckerFactory>());

    const auto component_list = userver::components::MinimalServerComponentList()
                                    .Append<AuthCache>()
                                    .Append<userver::components::Postgres>("auth-database")
                                    .Append<GetForModeration>()
                                    .Append<DeleteFromModeration>()
                                    .Append<AddObj>()
                                    .Append<userver::components::TestsuiteSupport>()
                                    .Append<userver::clients::dns::Component>();
    return userver::utils::DaemonMain(charc, charv, component_list);
}