cmake_minimum_required(VERSION 3.20)
project(YandexTransportCatalogue)

set(CMAKE_CXX_STANDARD 17)

add_executable(YandexTransportCatalogue
        main.cpp
        geo.h geo.cpp
        transport_catalogue.h transport_catalogue.cpp
        domain.h
        json.h json.cpp
        json_reader.h json_reader.cpp
        request_handler.h request_handler.cpp
        svg.h svg.cpp
        map_renderer.h map_renderer.cpp)
