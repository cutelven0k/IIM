cmake_minimum_required(VERSION 3.20)
project(IIM LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# creating build/compile_commands.json (I use it for vscode)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
#

include(FetchContent)
# userver fetching
set(USERVER_FEATURE_POSTGRESQL ON CACHE BOOL "Enable PostgreSQL support")
FetchContent_Declare(
    userver
    GIT_REPOSITORY https://github.com/userver-framework/userver.git
    GIT_TAG v2.7
)
FetchContent_MakeAvailable(userver)
#

userver_setup_environment()


# utils
## string_json_response
add_library(${PROJECT_NAME}_string_json_response_objs OBJECT
    src/utils/string_json_response.hpp
    src/utils/string_json_response.cpp
)
target_link_libraries(${PROJECT_NAME}_string_json_response_objs
    userver::core
)
target_include_directories(${PROJECT_NAME}_string_json_response_objs PUBLIC src/utils)
##
## json_process
add_library(${PROJECT_NAME}_json_process_objs OBJECT
    src/utils/json_process.hpp
    src/utils/json_process.cpp
)
target_link_libraries(${PROJECT_NAME}_json_process_objs
    ${PROJECT_NAME}_string_json_response_objs
    userver::core
)
target_include_directories(${PROJECT_NAME}_json_process_objs PUBLIC src/utils)
##
#
#
# handlers
## auth
### bearer
add_library(${PROJECT_NAME}_auth_objs OBJECT
    src/handlers/auth/auth_bearer.hpp
    src/handlers/auth/auth_bearer.cpp
    src/handlers/auth/user_info_cache.hpp
)
target_link_libraries(${PROJECT_NAME}_auth_objs
    ${PROJECT_NAME}_string_json_response_objs
    userver::core
    userver::postgresql
)
target_include_directories(${PROJECT_NAME}_auth_objs PUBLIC src/handlers/auth)
###
##
## moderation
add_library(${PROJECT_NAME}_moderation_objs OBJECT
    src/handlers/moderation/moderation.hpp
    src/handlers/moderation/moderation.cpp
)
target_link_libraries(${PROJECT_NAME}_moderation_objs
    ${PROJECT_NAME}_string_json_response_objs
    ${PROJECT_NAME}_json_process_objs
    userver::core
    userver::postgresql
)
target_include_directories(${PROJECT_NAME}_moderation_objs PUBLIC src/handlers/moderation)
##
#




add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME}
    ${PROJECT_NAME}_auth_objs
    ${PROJECT_NAME}_moderation_objs
    ${PROJECT_NAME}_string_json_response_objs
    ${PROJECT_NAME}_json_process_objs
    userver::postgresql
)

userver_testsuite_add_simple()
