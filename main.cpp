#include <auth_bearer.hpp>
#include <user_info_cache.hpp>
#include <role_test_handlers.hpp>
#include <sign_in_up.hpp>

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
                                    .Append<SignIn>()
                                    .Append<SignUp>()
                                    .Append<UserHandler>()
                                    .Append<AdminHandler>()
                                    .Append<ModeratorHandler>()
                                    .Append<userver::components::TestsuiteSupport>()
                                    .Append<userver::clients::dns::Component>();
    return userver::utils::DaemonMain(charc, charv, component_list);
}
