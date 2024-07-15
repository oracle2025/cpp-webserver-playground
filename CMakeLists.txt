cmake_minimum_required(VERSION 3.18)
project(cpp_webserver_kata)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-fsanitize=address,undefined)
add_link_options(-fsanitize=address,undefined)

#run clang-tidy only on linux
if (UNIX AND NOT APPLE)
    # set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-checks=*")
endif ()

set(CMAKE_PREFIX_PATH ${CMAKE_CURRENT_BINARY_DIR}/lib/cmake)

include(FeatureSummary)
include(GNUInstallDirs)

set(TODO_DATABASE_DIR ${CMAKE_INSTALL_FULL_LOCALSTATEDIR}/todo)
add_definitions(-DTODO_DATABASE_DIR="${TODO_DATABASE_DIR}")
install(DIRECTORY DESTINATION ${TODO_DATABASE_DIR})

set(TIME_RECORDING_DATABASE_DIR ${CMAKE_INSTALL_FULL_LOCALSTATEDIR}/time_recording)
add_definitions(-DTIME_RECORDING_DATABASE_DIR="${TIME_RECORDING_DATABASE_DIR}")
install(DIRECTORY DESTINATION ${TIME_RECORDING_DATABASE_DIR})

# Provide define with CMAKE config dir
# if build mode is debug, set config dir to the users home folder
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DCONFIG_DIR="$ENV{HOME}/.todo")
    add_definitions(-DTEMPLATE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/html")
    add_definitions(-DDEBUG_BUILD)
    if (APPLE)
        add_definitions(-fno-omit-frame-pointer -mno-omit-leaf-frame-pointer)
    endif ()
else ()
    set(TEMPLATE_DIR "${CMAKE_INSTALL_FULL_DATAROOTDIR}/todo")
    add_definitions(-DCONFIG_DIR="${CMAKE_INSTALL_FULL_SYSCONFDIR}/todo")
    add_definitions(-DTEMPLATE_DIR="${TEMPLATE_DIR}")
    install(DIRECTORY html/ DESTINATION ${TEMPLATE_DIR}
            FILES_MATCHING PATTERN "*.html")
endif ()



set(SERVER_PORT 8080)
option(SERVER_PORT "Port to listen on" ${SERVER_PORT})
add_definitions(-DSERVER_PORT=${SERVER_PORT})

set(SERVER_PORT_TIME_RECORDING 8081)
option(SERVER_PORT_TIME_RECORDING "Port to listen on" ${SERVER_PORT_TIME_RECORDING})
add_definitions(-DSERVER_PORT_TIME_RECORDING=${SERVER_PORT_TIME_RECORDING})

option(ENABLE_SIGNUP "Enable signup" OFF)
if (ENABLE_SIGNUP)
    add_definitions(-DENABLE_SIGNUP)
endif ()

option(ENABLE_USER_LIST "Enable user list" OFF)
if (ENABLE_USER_LIST)
    add_definitions(-DENABLE_USER_LIST)
endif ()

option(ENABLE_BACKWARD "Enable backward stack tracing library" ON)

find_package(Threads)
if (APPLE)
    find_package(Poco COMPONENTS Net Util Data DataSQLite NetSSL)
else ()
    find_package(OpenSSL)
    find_package(Poco COMPONENTS Net Util Data DataSQLite Crypto NetSSL)
    add_definitions(-DUSE_POCO_CRYPTO)
endif ()
find_package(spdlog)

include(FetchContent)
# Check for libbackward-cpp-dev
if (ENABLE_BACKWARD)
    # Also requires one of: libbfd (gnu binutils), libdwarf, libdw (elfutils)
    find_package(Backward HINTS /usr/lib/backward)
    if (NOT ${Backward_FOUND})
        FetchContent_Declare(
                backward
                GIT_REPOSITORY https://github.com/bombela/backward-cpp
                GIT_TAG v1.6)
        FetchContent_MakeAvailable(backward)
        set(BACKWARD_OBJECT ${BACKWARD_ENABLE})
    else ()
        set(BACKWARD_LIBRARY_TARGET Backward::Backward)
    endif ()
endif ()

include_directories(SYSTEM ${PROJECT_BINARY_DIR}/include)
include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/lib/include)
link_directories(${PROJECT_BINARY_DIR}/lib)

if (${Poco_FOUND} AND ${spdlog_FOUND})

    add_library(
            webservers OBJECT
            src/Data/Todo.cpp
            src/Data/Todo.hpp
            src/Data/SharedTodo.cpp
            src/Data/SharedTodo.hpp
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
            src/Http/SessionId.cpp
            src/Http/SessionId.hpp
            src/Login/LoginController.hpp
            src/Server/CrudController.hpp
            src/Impl/PocoWebServer.cpp
            src/Impl/PocoWebServer.hpp
            src/Login/LoginServerApplication.cpp
            src/Login/LoginServerApplication.hpp
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
            src/Data/V2/UserV2.cpp
            src/Data/V2/UserV2.hpp
            src/Server/Confirm.cpp
            src/Server/Confirm.hpp
            src/Data/MigrationsV0.cpp
            src/Data/MigrationsV0.hpp
            src/Data/MigrationsV5.cpp
            src/Data/MigrationsV5.hpp
            src/Data/Migration.cpp
            src/Data/Migration.hpp
            src/Data/SessionImpl.cpp
            src/Data/MigrationsV1.cpp
            src/Data/MigrationsV1.hpp
            src/Trace/trace.hpp
            src/Trace/Exception.cpp
            src/Trace/Exception.hpp
            ${BACKWARD_OBJECT}
            src/User/PasswordChangeController.hpp
            src/Http/NullHandler.cpp
            src/Http/NullHandler.hpp
            src/Signup/SignupController.hpp
            src/Filter/ByOwner.cpp
            src/Filter/ByOwner.hpp
            src/Data/MigrationsV2.cpp
            src/Data/MigrationsV2.hpp
            src/Data/PasswordSalting.cpp
            src/Data/PasswordSalting.hpp
            src/Data/MigrationsV3.cpp
            src/Data/MigrationsV3.hpp
            src/String/escape.hpp
            src/String/escape.cpp
            src/Impl/PocoPageHandler.cpp
            src/Impl/PocoPageHandler.hpp
            src/Proto/Event.cpp
            src/Proto/Event.hpp
            src/Input/Date.cpp
            src/Input/Date.h
            src/Input/TimeElement.cpp
            src/Proto/ScaffoldRecord.cpp
            src/Proto/ScaffoldRecord.hpp
            src/Data/Date.cpp
            src/Data/Date.hpp
            src/String/split.hpp
            src/String/split.cpp
            src/String/capitalize.hpp
            src/String/capitalize.cpp
            src/Server/CrudController.cpp
            src/Data/MigrationsV4.cpp
            src/Data/MigrationsV4.hpp
            src/Login/LoginController.cpp
            src/Http/handler_type.hpp
            src/Http/RouteId.cpp
            src/Http/RouteId.hpp
            src/Signup/SignupController.cpp
            src/User/PasswordChangeController.cpp
            src/Server/TodoController.cpp
            src/Server/TodoController.hpp
            src/Template/BaseTemplate.cpp
            src/Template/BaseTemplate.hpp
            src/Data/AsJson.cpp
            src/Data/AsJson.hpp
            src/Data/Migrations.hpp
            src/Login/ProfileController.cpp
            src/Login/ProfileController.hpp
            src/Document/DocumentController.cpp
            src/Document/DocumentController.hpp
            src/Http/RequestHandlerList.hpp
            src/Data/Movie.cpp
            src/Data/Movie.hpp
            src/Data/MigrationsV6.cpp
            src/Data/MigrationsV6.hpp
            src/Server/MovieController.cpp
            src/Server/MovieController.hpp
            src/TimeRecordingApp/TimeEntryController.cpp
            src/TimeRecordingApp/TimeEntryController.hpp
            src/TimeRecordingApp/TimeCorrectionController.cpp
            src/TimeRecordingApp/TimeCorrectionController.hpp
            src/TimeRecordingApp/TimeReportController.cpp
            src/TimeRecordingApp/TimeReportController.hpp
            src/TimeRecordingApp/TimeEntry.cpp
            src/TimeRecordingApp/TimeEntry.hpp
            src/TimeRecordingApp/TimeRecordingApplication.cpp
            src/TimeRecordingApp/TimeRecordingApplication.hpp
            src/Http/SessionStorage.cpp
            src/Http/SessionStorage.hpp
            src/Data/SessionRow.cpp
            src/Data/SessionRow.hpp
            src/Data/MigrationsV7.cpp
            src/Data/MigrationsV7.hpp
            src/Http/SessionStorageSql.cpp
            src/Http/SessionStorageSql.hpp
            src/Data/MigrationsV8.cpp
            src/Data/MigrationsV8.hpp
            src/String/currentDateTime.cpp
            src/String/currentDateTime.hpp
            src/String/timeDifference.cpp
            src/String/timeDifference.hpp
            src/Time/Time.cpp
            src/Time/Time.hpp)
    target_compile_definitions(
            webservers PUBLIC -DCURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
    target_compile_options(webservers PRIVATE -Wall -Wextra -Werror -Wpedantic
            -Wno-unused-parameter -Wno-unused-variable
            -Wno-unused-but-set-variable -Wno-unused-function
            -Wno-unused-lambda-capture
            # -Wno-unused-local-typedef
            -Wno-unused-value -Wno-unused-const-variable
            -Wfloat-equal
            # -Wconversion
            # -Wundef
            -Wredundant-decls
            # -Wshadow
            -Wstrict-overflow=2
            -Wwrite-strings
            -Wpointer-arith
            -Wcast-qual
            -Wformat=2
            # -Wmissing-include-dirs
            -Wcast-align
            -Wswitch-enum
            #-Wsign-conversion
            -Wdisabled-optimization

            -Winvalid-pch
            # -Wmissing-declarations
            -Wdouble-promotion
            -Wshift-overflow
            -Wnull-dereference
            -Wcast-align)
    target_link_libraries(
            webservers
            Poco::Net
            Poco::Util
            Poco::Data
            Poco::NetSSL
            Threads::Threads
            Poco::DataSQLite
            spdlog::spdlog
            ${BACKWARD_LIBRARY_TARGET})
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
            tests/LambdaTest.cpp
            tests/SessionIdTest.cpp
            tests/InputTest.cpp
            tests/WebServerTest.cpp
            tests/ResponseTest.cpp
            tests/CrudServerTest.cpp
            tests/TodoTest.cpp
            tests/TimeRecordingSchema.cpp
            tests/AlertTest.cpp
            tests/RequestTest.cpp
            tests/PasswordChangeTest.cpp
            tests/SignupTest.cpp
            tests/ByOwnerTest.cpp
            tests/PocoPageHandlerTest.cpp tests/FakeBrowser.cpp tests/FakeBrowser.hpp
            tests/LambdaTest.cpp
            tests/MovieTest.cpp
            tests/TimeRecordingSchema.cpp
            tests/SessionTest.cpp
            tests/TimeEntryTest.cpp
            tests/TimeConvertTest.cpp)
    target_link_libraries(tests webservers)
    target_include_directories(tests PRIVATE tests)


    add_executable(tests-performance tests/performance/TodoPerformanceTest.cpp tests/doctest.cpp)
    target_include_directories(tests-performance PRIVATE tests)
    target_link_libraries(tests-performance webservers)

    enable_testing()
    add_test(NAME tests COMMAND $<TARGET_FILE:tests>)


    add_executable(login-server src/Login/LoginMain.cpp tests/doctest_impl.cpp)
    target_link_libraries(login-server webservers)

    add_executable(time-recording-server src/TimeRecordingApp/TimeRecordingMain.cpp tests/doctest_impl.cpp)
    target_link_libraries(time-recording-server webservers)


    install(TARGETS login-server DESTINATION bin)
    install(TARGETS time-recording-server DESTINATION bin)

    configure_file(systemd/todo.service.in systemd/todo.service @ONLY)

    if (ENABLE_BACKWARD)
        add_backward(webservers)
        add_backward(tests)
        add_backward(login-server)
        add_backward(time-recording-server)
        add_backward(tests-performance)
    endif ()
endif ()

include(ExternalProject)

if (NOT ${Poco_FOUND})
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
endif ()

if (NOT ${spdlog_FOUND})
    ExternalProject_Add(
            spdlog
            GIT_REPOSITORY https://github.com/gabime/spdlog.git
            GIT_TAG v1.11.0
            GIT_SHALLOW ON
            GIT_PROGRESS OFF
            INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}
            CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR})
endif ()

message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Install Prefix: ${CMAKE_INSTALL_PREFIX}")

message(STATUS "BINDIR: ${CMAKE_INSTALL_FULL_BINDIR}")
message(STATUS "SBINDIR: ${CMAKE_INSTALL_FULL_SBINDIR}")
message(STATUS "LIBEXECDIR: ${CMAKE_INSTALL_FULL_LIBEXECDIR}")
message(STATUS "SYSCONFDIR: ${CMAKE_INSTALL_FULL_SYSCONFDIR}")
message(STATUS "SHAREDSTATEDIR: ${CMAKE_INSTALL_FULL_SHAREDSTATEDIR}")
message(STATUS "LOCALSTATEDIR: ${CMAKE_INSTALL_FULL_LOCALSTATEDIR}")
message(STATUS "RUNSTATEDIR: ${CMAKE_INSTALL_FULL_RUNSTATEDIR}")
message(STATUS "LIBDIR: ${CMAKE_INSTALL_FULL_LIBDIR}")
message(STATUS "INCLUDEDIR: ${CMAKE_INSTALL_FULL_INCLUDEDIR}")
message(STATUS "OLDINCLUDEDIR: ${CMAKE_INSTALL_FULL_OLDINCLUDEDIR}")
message(STATUS "DATAROOTDIR: ${CMAKE_INSTALL_FULL_DATAROOTDIR}")
message(STATUS "DATADIR: ${CMAKE_INSTALL_FULL_DATADIR}")
message(STATUS "INFODIR: ${CMAKE_INSTALL_FULL_INFODIR}")
message(STATUS "LOCALEDIR: ${CMAKE_INSTALL_FULL_LOCALEDIR}")
message(STATUS "MANDIR: ${CMAKE_INSTALL_FULL_MANDIR}")
message(STATUS "DOCDIR: ${CMAKE_INSTALL_FULL_DOCDIR}")


feature_summary(WHAT ALL)

set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Richard Spindler")
include(CPack)