cmake_minimum_required(VERSION 3.18.0)
project(transport_catalogue)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_PREFIX_PATH "C:/Users/Kaise/CLionProjects/protobuf-cpp-3.20.0/package")

find_package(Protobuf REQUIRED)
find_package(Threads REQUIRED)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS
        svg.proto
        map_renderer.proto
        transport_catalogue.proto)

add_executable(transport_catalogue ${PROTO_SRCS} ${PROTO_HDRS}
        main.cpp
        geo.h geo.cpp
        transport_catalogue.h transport_catalogue.cpp
        domain.h
        json.h json.cpp
        json_reader.h json_reader.cpp
        request_handler.h request_handler.cpp
        svg.h svg.cpp
        map_renderer.h map_renderer.cpp
        json_builder.h json_builder.cpp
        graph.h ranges.h router.h
        transport_router.h transport_router.cpp
        serialization.h serialization.cpp)

target_include_directories(transport_catalogue PUBLIC ${Protobuf_INCLUDE_DIRS})
target_include_directories(transport_catalogue PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

string(REPLACE "protobuf.lib" "protobufd.lib" "Protobuf_LIBRARY_DEBUG" "${Protobuf_LIBRARY_DEBUG}")
string(REPLACE "protobuf.a" "protobufd.a" "Protobuf_LIBRARY_DEBUG" "${Protobuf_LIBRARY_DEBUG}")

target_link_libraries(transport_catalogue "$<IF:$<CONFIG:Debug>,${Protobuf_LIBRARY_DEBUG},${Protobuf_LIBRARY}>" Threads::Threads)
