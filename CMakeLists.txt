cmake_minimum_required(VERSION 3.18)
project(cpp_webserver_kata)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_PREFIX_PATH ${CMAKE_CURRENT_BINARY_DIR}/lib/cmake)

find_package(Threads)
find_package(Poco COMPONENTS Net Util Data DataSQLite)

include_directories(${PROJECT_BINARY_DIR}/include)
link_directories(${PROJECT_BINARY_DIR}/lib)

if (${Poco_FOUND})
    add_executable(cpp_webserver_kata src/main.cpp src/Page.cpp src/Page.hpp)
    target_link_libraries(cpp_webserver_kata Poco::Net Poco::Util
            Threads::Threads)

    add_library(webservers OBJECT  src/Todo.cpp src/Todo.hpp src/Table.cpp src/Table.hpp
            src/Record.cpp src/RecursiveWebServer.cpp src/RecursiveWebServer.hpp
            src/Record.hpp src/Form.cpp src/Form.hpp src/List.cpp src/List.hpp src/Text.cpp src/Text.hpp src/Hidden.cpp
            src/Hidden.hpp src/Password.cpp src/Password.hpp src/Submit.cpp src/Submit.hpp src/Response.cpp
            src/Response.hpp src/Request.cpp src/Request.hpp src/TestServer.cpp src/TestServer.hpp src/overloaded.hpp
            src/SessionId.cpp src/SessionId.hpp src/LoginServer.hpp src/CrudServer.hpp src/CrudServer.cpp
            src/LoginServer.cpp src/PocoWebServer.cpp src/PocoWebServer.hpp src/LoginServerApplication.cpp
            src/LoginServerApplication.hpp src/CrudServerApplication.cpp src/CrudServerApplication.hpp src/RequestHandler.hpp src/style.hpp src/FieldTypes.hpp)

    target_link_libraries(webservers Poco::Net Poco::Util Poco::Data
            Threads::Threads Poco::DataSQLite)
    target_include_directories(webservers
            PRIVATE tests
            PUBLIC src
            )

    add_executable(tests tests/doctest.cpp tests/LoginServerTest.cpp tests/SessionIdTest.cpp tests/InputTest.cpp tests/WebServerTest.cpp tests/ResponseTest.cpp tests/PocoWebServerTest.cpp tests/CrudServerTest.cpp tests/TodoTest.cpp)
    target_link_libraries(tests webservers)
    target_include_directories(tests PRIVATE tests)

    add_executable(crud-server src/CrudMain.cpp tests/doctest_impl.cpp)
    target_link_libraries(crud-server webservers)
    add_executable(login-server src/LoginMain.cpp tests/doctest_impl.cpp)
    target_link_libraries(login-server webservers)
    install(TARGETS login-server DESTINATION bin)
endif ()

include(ExternalProject)

if (NOT ${Poco_FOUND})
    ExternalProject_Add(libpoco
            GIT_REPOSITORY https://github.com/pocoproject/poco.git
            GIT_TAG poco-1.10.1-release
            GIT_SHALLOW ON
            GIT_PROGRESS OFF
            INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}
            CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}
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
            -DENABLE_DATA=ON
            )
endif ()