cmake_minimum_required(VERSION 3.18)
project(cpp_webserver_kata)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-fsanitize=address,undefined)
add_link_options(-fsanitize=address,undefined)

set(CMAKE_PREFIX_PATH ${CMAKE_CURRENT_BINARY_DIR}/lib/cmake)

include(GNUInstallDirs)
set(TODO_DATABASE_DIR ${CMAKE_INSTALL_FULL_LOCALSTATEDIR}/todo)
add_definitions(-DTODO_DATABASE_DIR="${TODO_DATABASE_DIR}")

install(DIRECTORY DESTINATION ${TODO_DATABASE_DIR})

set(SERVER_PORT 8080)
option(SERVER_PORT "Port to listen on" ${SERVER_PORT})
add_definitions(-DSERVER_PORT=${SERVER_PORT})

option(ENABLE_SIGNUP "Enable signup" OFF)
if(ENABLE_SIGNUP)
  add_definitions(-DENABLE_SIGNUP)
endif()

option(ENABLE_USER_LIST "Enable user list" OFF)
if(ENABLE_USER_LIST)
  add_definitions(-DENABLE_USER_LIST)
endif()

option(ENABLE_BACKWARD "Enable backward stack tracing library" ON)

find_package(Threads)
if(APPLE)
  find_package(Poco COMPONENTS Net Util Data DataSQLite)
else()
  find_package(Poco COMPONENTS Net Util Data DataSQLite Crypto)
  add_definitions(-DUSE_POCO_CRYPTO)
endif()
find_package(spdlog)

include(FetchContent)

if(ENABLE_BACKWARD)
  # Also requires one of: libbfd (gnu binutils), libdwarf, libdw (elfutils)
  FetchContent_Declare(
    backward
    GIT_REPOSITORY https://github.com/bombela/backward-cpp
    GIT_TAG v1.6)
  FetchContent_MakeAvailable(backward)
endif()

include_directories(${PROJECT_BINARY_DIR}/include)
link_directories(${PROJECT_BINARY_DIR}/lib)

if(${Poco_FOUND} AND ${spdlog_FOUND})

  add_library(
    webservers OBJECT
    src/stb/stb_image.h
    src/stb/stb_image_resize.h
    src/stb/stb_image_write.h
    src/stb/stb_implementation.cpp
    src/Data/Todo.cpp
    src/Data/Todo.hpp
    src/Html/Table.cpp
    src/Html/Table.hpp
    src/Data/Record.cpp
    src/Impl/SimpleWebServer.cpp
    src/Impl/SimpleWebServer.hpp
    src/Data/Record.hpp
    src/Input/Form.cpp
    src/Input/Form.hpp
    src/Html/List.cpp
    src/Html/List.hpp
    src/Input/Text.cpp
    src/Input/Text.hpp
    src/Input/Hidden.cpp
    src/Input/Hidden.hpp
    src/Input/Password.cpp
    src/Input/Password.hpp
    src/Input/Submit.cpp
    src/Input/Submit.hpp
    src/Http/Response.cpp
    src/Http/Response.hpp
    src/Http/Request.cpp
    src/Http/Request.hpp
    src/Server/TestServer.cpp
    src/Server/TestServer.hpp
    src/Http/SessionId.cpp
    src/Http/SessionId.hpp
    src/Login/LoginController.hpp
    src/Server/CrudController.hpp
    src/Impl/PocoWebServer.cpp
    src/Impl/PocoWebServer.hpp
    src/Login/LoginServerApplication.cpp
    src/Login/LoginServerApplication.hpp
    src/Server/CrudServerApplication.cpp
    src/Server/CrudServerApplication.hpp
    src/Http/RequestHandler.hpp
    src/Server/style.hpp
    src/Data/FieldTypes.hpp
    src/Html/Presentation.cpp
    src/Html/Presentation.hpp
    src/Html/Page.cpp
    src/Html/Page.hpp
    src/Html/ActionLink.cpp
    src/Html/ActionLink.hpp
    src/Input/CheckBoxSelect.cpp
    src/Input/CheckBoxSelect.hpp
    src/Html/AlertType.hpp
    src/Html/Alert.cpp
    src/Html/Alert.hpp
    src/Http/Router.cpp
    src/Http/Router.hpp
    src/Http/NotFoundHandler.hpp
    src/Http/NotFoundHandler.cpp
    src/Data/RecordFactory.hpp
    src/Data/Person.cpp
    src/Data/Person.hpp
    src/String/join.cpp
    src/String/join.hpp
    src/String/repeat.hpp
    src/String/repeat.cpp
    src/Data/RecordImpl.hpp
    src/String/createRandomUUID.cpp
    src/String/createRandomUUID.hpp
    src/Http/Session.cpp
    src/Http/Session.hpp
    src/Http/SessionData.cpp
    src/Http/SessionData.hpp
    src/Data/User.cpp
    src/Data/User.hpp
    src/Server/Confirm.cpp
    src/Server/Confirm.hpp
    src/Data/MigrationsV0.cpp
    src/Data/MigrationsV0.hpp
    src/Data/Migration.cpp
    src/Data/Migration.hpp
    src/Data/SessionImpl.cpp
    src/Data/MigrationsV1.cpp
    src/Data/MigrationsV1.hpp
    src/Trace/trace.hpp
    src/Trace/Exception.cpp
    src/Trace/Exception.hpp
    ${BACKWARD_ENABLE}
    src/User/PasswordChangeController.hpp
    src/Http/RequestDispatcher.cpp
    src/Http/RequestDispatcher.hpp
    src/Http/NullHandler.cpp
    src/Http/NullHandler.hpp
    src/Signup/SignupController.hpp
    src/Filter/ByOwner.cpp
    src/Filter/ByOwner.hpp
    src/Data/MigrationsV2.cpp
    src/Data/MigrationsV2.hpp
    src/ImageBrowser/ImageBrowserController.hpp
    src/ImageBrowser/ImageBrowserApplication.cpp
    src/Data/PasswordSalting.cpp
    src/Data/PasswordSalting.hpp
    src/Data/MigrationsV3.cpp
    src/Data/MigrationsV3.hpp
    src/Groups/GroupManagementApplication.cpp
    src/Groups/GroupManagementApplication.hpp
    src/String/escape.hpp
    src/String/escape.cpp
    src/Impl/PocoPageHandler.cpp
    src/Impl/PocoPageHandler.hpp)
  target_compile_definitions(
    webservers PUBLIC -DCURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

  target_link_libraries(
    webservers
    Poco::Net
    Poco::Util
    Poco::Data
    Threads::Threads
    Poco::DataSQLite
    spdlog::spdlog)
  target_include_directories(
    webservers
    PRIVATE tests
    PUBLIC src
    PUBLIC src/Html
    PUBLIC src/Input
    PUBLIC src/Impl
    PUBLIC src/Server
    PUBLIC src/Http)

  add_executable(
    tests
    tests/doctest.cpp
    tests/LoginServerTest.cpp
    tests/SessionIdTest.cpp
    tests/InputTest.cpp
    tests/WebServerTest.cpp
    tests/ResponseTest.cpp
    tests/PocoWebServerTest.cpp
    tests/CrudServerTest.cpp
    tests/TodoTest.cpp
    tests/AlertTest.cpp
    tests/RequestTest.cpp
    tests/PasswordChangeTest.cpp
    tests/SignupTest.cpp
    tests/ByOwnerTest.cpp
    tests/ImageBrowserTest.cpp
    tests/PocoPageHandlerTest.cpp)
  target_link_libraries(tests webservers)
  target_include_directories(tests PRIVATE tests)

  add_executable(test-migration src/Data/Migration.cpp tests/doctest.cpp
                                src/Data/SessionImpl.cpp)
  target_link_libraries(test-migration Poco::DataSQLite)
  target_include_directories(test-migration PRIVATE tests)

  enable_testing()
  add_test(NAME tests COMMAND $<TARGET_FILE:tests>)
  add_test(NAME test-migration COMMAND $<TARGET_FILE:test-migration>)

  add_executable(crud-server src/Server/CrudMain.cpp tests/doctest_impl.cpp
                             src/User/PasswordChangeController.hpp)
  target_link_libraries(crud-server webservers)

  add_executable(login-server src/Login/LoginMain.cpp tests/doctest_impl.cpp)
  target_link_libraries(login-server webservers)

  add_executable(image-browser-server src/ImageBrowser/ImageBrowserMain.cpp
                                      tests/doctest_impl.cpp)
  target_link_libraries(image-browser-server webservers)

  install(TARGETS login-server DESTINATION bin)

  if(ENABLE_BACKWARD)
    add_backward(webservers)
    add_backward(tests)
    add_backward(test-migration)
    add_backward(crud-server)
    add_backward(login-server)
    add_backward(image-browser-server)
  endif()
endif()

include(ExternalProject)

if(NOT ${Poco_FOUND})
  ExternalProject_Add(
    libpoco
    GIT_REPOSITORY https://github.com/pocoproject/poco.git
    GIT_TAG poco-1.12.4-release
    GIT_SHALLOW ON
    GIT_PROGRESS OFF
    INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}
               -DENABLE_ENCODINGS=OFF
               -DENABLE_XML=ON
               -DENABLE_JSON=ON
               -DENABLE_MONGODB=OFF
               -DENABLE_DATA_SQLITE=ON
               -DENABLE_REDIS=OFF
               -DENABLE_PDF=OFF
               -DENABLE_ZIP=OFF
               -DENABLE_PAGECOMPILER=OFF
               -DENABLE_PAGECOMPILER_FILE2PAGE=OFF
               -DENABLE_DATA=ON)
endif()

if(NOT ${spdlog_FOUND})
  ExternalProject_Add(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.11.0
    GIT_SHALLOW ON
    GIT_PROGRESS OFF
    INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR})
endif()
