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
    GIT_TAG develop
)

FetchContent_MakeAvailable(userver)
#

# Вызов настройки окружения userver
userver_setup_environment()
#
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} userver::postgresql)

# config connection to the project
userver_embed_file(${PROJECT_NAME}_config
    NAME static_config_yaml
    FILEPATH static_config.yaml
)
target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}_config)
#
