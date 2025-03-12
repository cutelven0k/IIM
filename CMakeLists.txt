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

userver_setup_environment()

# handlers
## auth
### 
### bearer
add_library(${PROJECT_NAME}_auth_objs OBJECT
    src/handlers/auth/auth_bearer.hpp
    src/handlers/auth/auth_bearer.cpp
    src/handlers/auth/user_info_cache.hpp
)
target_link_libraries(${PROJECT_NAME}_auth_objs userver::core userver::postgresql)
target_include_directories(${PROJECT_NAME}_auth_objs PUBLIC src/handlers/auth)
###
### sign_in_up
add_library(${PROJECT_NAME}_sign_in_up_objs OBJECT
    src/handlers/auth/sign_in_up.hpp
    src/handlers/auth/sign_in_up.cpp
)
target_link_libraries(${PROJECT_NAME}_sign_in_up_objs userver::core userver::postgresql)
target_include_directories(${PROJECT_NAME}_sign_in_up_objs PUBLIC src/handlers/auth)
###
### role_test_handlers
add_library(${PROJECT_NAME}_role_test_handlers_objs OBJECT
    src/handlers/handlers_for_test/role_test_handlers.hpp
    src/handlers/handlers_for_test/role_test_handlers.cpp
)
target_link_libraries(${PROJECT_NAME}_role_test_handlers_objs userver::core)
target_include_directories(${PROJECT_NAME}_role_test_handlers_objs PUBLIC src/handlers/handlers_for_test)
###
##
#

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME}
    ${PROJECT_NAME}_auth_objs
    ${PROJECT_NAME}_role_test_handlers_objs
    ${PROJECT_NAME}_sign_in_up_objs
    userver::postgresql
)

userver_testsuite_add_simple()
