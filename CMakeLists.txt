cmake_minimum_required(VERSION 3.18)
project(cpp_webserver_kata)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_PREFIX_PATH ${CMAKE_CURRENT_BINARY_DIR}/lib/cmake)

find_package(Threads)
find_package(Poco COMPONENTS Net Util Data DataSQLite)

include_directories(${PROJECT_BINARY_DIR}/include)
link_directories(${PROJECT_BINARY_DIR}/lib)

if(${Poco_FOUND})
    add_executable(cpp_webserver_kata main.cpp Page.cpp Page.hpp)
    target_link_libraries(cpp_webserver_kata Poco::Net Poco::Util
            Threads::Threads)

    add_executable(tests tests.cpp doctest.cpp todo.cpp todo.hpp Table.cpp Table.hpp Record.cpp Record.hpp Form.cpp Form.hpp List.cpp List.hpp)
    target_link_libraries(tests Poco::Net Poco::Util Poco::Data
            Threads::Threads Poco::DataSQLite)
endif()

include(ExternalProject)

if(NOT ${Poco_FOUND})
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
endif()