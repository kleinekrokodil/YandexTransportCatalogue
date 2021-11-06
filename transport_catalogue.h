#pragma once
#include "geo.h"
#include <unordered_map>
#include <string_view>
#include <string>
#include <deque>
#include <vector>
#include <functional>
#include <set>

struct Stop{
    std::string_view stop_name;
    double latitude = 0.0;
    double longitude = 0.0;
};

struct Bus{
    std::string_view bus_name;
    std::vector<const Stop*> route;
    bool is_circle = false;
    double r_length = 0.0;
};

struct BusRoute{
    std::string_view bus_name;
    size_t stops = 0;
    size_t unique_stops = 0;
    double r_length = 0.0;
    bool is_found = false;
};

struct StopRoutes{
    std::string_view stop_name;
    std::set<std::string_view> routes;
    bool is_found = false;
};


void RemoveBeginEndSpaces(std::string_view& str);

std::string FindName(std::string& str, char separator);

class TransportCatalogue{
public:
    TransportCatalogue() = default;
    explicit TransportCatalogue(std::deque<std::string> q);

    void AddStop(std::string_view stop_sv);

    void AddBus(std::string_view bus_sv);

    Stop FindStop(std::string& stop);

    Bus FindBus(std::string& bus);

    BusRoute RouteInformation(std::string_view bus);

    StopRoutes StopInformation(std::string_view stop);



private:
    std::deque<std::string> queries_;
    std::unordered_map<std::string_view, Stop> stops_;
    std::unordered_map<std::string_view, Bus> buses_;
    std::unordered_map<std::string_view, std::set<std::string_view>> buses_for_stops_;
};