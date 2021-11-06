#include "transport_catalogue.h"
#include <functional>
#include <cstdint>
#include <set>

void RemoveBeginEndSpaces(std::string_view& str){
    while(str.front() == ' '){
        str.remove_prefix(1);
    }
    while(str.back() == ' '){
        str.remove_suffix(1);
    }
}

std::string_view FindName(std::string_view& sv, char separator){
    std::string_view name;
    std::uint64_t sep_pos;
    if(sv.find(separator) != sv.npos){ //Ищем признак окончания названия
        sep_pos = sv.find(separator);
    }
    else{
        sep_pos = sv.npos;
    }
    name = sv.substr(0, sep_pos);
    RemoveBeginEndSpaces(name); //Убираем начальные и конечные пробелы
    if(sep_pos != sv.npos){
        sv.remove_prefix(sep_pos + 1);
    }
    else{
        sv.remove_prefix(sv.size());
    }
    return name;
}

TransportCatalogue::TransportCatalogue(std::deque<std::string> q)
:queries_(std::move(q))
{
    for(const auto& i : queries_){
        if(i.substr(0, 4) == "Stop"){
            AddStop(i);
        }
        else if(i.substr(0, 3) == "Bus"){
            AddBus(i);
        }
    }
}

void TransportCatalogue::AddStop(std::string_view stop_sv){
    Stop stop;
    //std::string_view stop_sv = sv;
    //Находим имя остановки
    stop_sv.remove_prefix(4); //Убираем слово Stop
    stop.stop_name = FindName(stop_sv, ':');
    //Преобразуем строковые значения широты и долготы в числовые
    std::string_view lat_ = FindName(stop_sv, ',');
    stop.latitude = std::stod({lat_.data(), lat_.size()});
    std::string_view lng_ = FindName(stop_sv, ',');
    stop.longitude = std::stod({lng_.data(), lng_.size()});
    stops_.insert({stop.stop_name, stop});
}

void TransportCatalogue::AddBus(std::string_view bus_sv){
    Bus bus;
    bus_sv.remove_prefix(3);
    bus.bus_name = FindName(bus_sv, ':');
    char sep = '-';
    if(bus_sv.find('>') != bus_sv.npos){
        bus.is_circle = true;
        sep = '>';
    }
    while(!bus_sv.empty()){
        auto stop_name = FindName(bus_sv, sep);
        if(stops_.count(stop_name)){
            Stop* stop_ptr = &stops_.at(stop_name);
            bus.route.push_back(stop_ptr);
        }
    }
    for(size_t i = 1; i < bus.route.size(); ++i){
        if(bus.route[i - 1] == bus.route[i]){
            continue;
        }
        else{
            bus.r_length += ComputeDistance({bus.route[i - 1]->latitude, bus.route[i - 1]->longitude},
                                            {bus.route[i]->latitude, bus.route[i]->longitude});
        }
    }
    if(!bus.is_circle){
        bus.r_length *= 2;
    }
    buses_.insert({bus.bus_name, bus});
}

Stop TransportCatalogue::FindStop(std::string& stop){
    return stops_.at(stop);
}

Bus TransportCatalogue::FindBus(std::string& bus){
    return buses_.at(bus);
}

BusRoute TransportCatalogue::RouteInformation(std::string_view bus){
    RemoveBeginEndSpaces(bus);
    BusRoute route;
    if(buses_.count(bus)){
        route.is_found = true;
        std::set<const Stop*> unique_stops(buses_.at(bus).route.begin(),buses_.at(bus).route.end() );
        route.bus_name = buses_.at(bus).bus_name;
        route.stops = (buses_.at(bus).is_circle) ? (buses_.at(bus).route.size()) : (buses_.at(bus).route.size() * 2 - 1);
        route.unique_stops = unique_stops.size();
        route.r_length = buses_.at(bus).r_length;
    }
    else{
        route.bus_name = bus;
    }
    return route;
}


