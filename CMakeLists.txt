cmake_minimum_required(VERSION 3.20)
project(YandexTransportCatalogue)

set(CMAKE_CXX_STANDARD 17)

add_executable(YandexTransportCatalogue main.cpp geo.h geo.cpp
        transport_catalogue.cpp transport_catalogue.h  domain.h json.h json.cpp json_reader.cpp json_reader.h
         request_handler.cpp request_handler.h )
