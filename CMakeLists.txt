cmake_minimum_required(VERSION 3.18)
project(cpp_webserver_kata)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_PREFIX_PATH ${CMAKE_CURRENT_BINARY_DIR}/lib/cmake)

find_package(Threads)
find_package(Poco COMPONENTS Net Util Data DataSQLite)

include_directories(${PROJECT_BINARY_DIR}/include)
link_directories(${PROJECT_BINARY_DIR}/lib)

if (${Poco_FOUND})

    add_library(webservers OBJECT src/Data/Todo.cpp src/Data/Todo.hpp src/Html/Table.cpp src/Html/Table.hpp
            src/Data/Record.cpp src/Server/RecursiveWebServer.cpp src/Server/RecursiveWebServer.hpp
            src/Data/Record.hpp src/Input/Form.cpp src/Input/Form.hpp src/Html/List.cpp src/Html/List.hpp src/Input/Text.cpp src/Input/Text.hpp src/Input/Hidden.cpp
            src/Input/Hidden.hpp src/Input/Password.cpp src/Input/Password.hpp src/Input/Submit.cpp src/Input/Submit.hpp src/Http/Response.cpp
            src/Http/Response.hpp src/Http/Request.cpp src/Http/Request.hpp src/Server/TestServer.cpp src/Server/TestServer.hpp
            src/Login/SessionId.cpp src/Login/SessionId.hpp src/Login/LoginServer.hpp src/Server/CrudServer.hpp src/Server/CrudServer.cpp
            src/Login/LoginServer.cpp src/Impl/PocoWebServer.cpp src/Impl/PocoWebServer.hpp src/Login/LoginServerApplication.cpp
            src/Login/LoginServerApplication.hpp src/Server/CrudServerApplication.cpp src/Server/CrudServerApplication.hpp
            src/Http/RequestHandler.hpp src/Server/style.hpp src/Data/FieldTypes.hpp src/Html/Presentation.cpp
            src/Html/Presentation.hpp src/Html/Page.cpp src/Html/Page.hpp src/Html/ActionLink.cpp src/Html/ActionLink.hpp src/Input/CheckBoxSelect.cpp src/Input/CheckBoxSelect.hpp src/Html/AlertType.hpp src/Html/Alert.cpp src/Html/Alert.hpp)

    target_link_libraries(webservers Poco::Net Poco::Util Poco::Data
            Threads::Threads Poco::DataSQLite)
    target_include_directories(webservers
            PRIVATE tests
            PUBLIC src
            PUBLIC src/Html
            PUBLIC src/Input
            PUBLIC src/Impl
            PUBLIC src/Server
            PUBLIC src/Http
            )

    add_executable(tests tests/doctest.cpp tests/LoginServerTest.cpp tests/SessionIdTest.cpp tests/InputTest.cpp tests/WebServerTest.cpp tests/ResponseTest.cpp tests/PocoWebServerTest.cpp tests/CrudServerTest.cpp tests/TodoTest.cpp)
    target_link_libraries(tests webservers)
    target_include_directories(tests PRIVATE tests)

    enable_testing()
    add_test(NAME tests COMMAND $<TARGET_FILE:tests>)

    add_executable(crud-server src/Server/CrudMain.cpp tests/doctest_impl.cpp)
    target_link_libraries(crud-server webservers)
    add_executable(login-server src/Login/LoginMain.cpp tests/doctest_impl.cpp)
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