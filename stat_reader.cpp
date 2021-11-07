#include "stat_reader.h"
#include <iostream>

using namespace std::string_literals;

void StatReader(TransportCatalogue t){
    int req_num;
    std::cin >> req_num;
    std::cin.ignore();
    for(int i = 0; i < req_num; ++i){
        std::string query;
        std::getline(std::cin, query);
        while (query[0] == ' '){
            query.erase(0, 1);
        }
        if(query.substr(0, 3) == "Bus"s){
            query.erase(0, 3);
            BusRoute r = t.RouteInformation(query);
            PrintBusInfo(r);
        }
        if(query.substr(0, 4) == "Stop"s){
            query.erase(0, 4);
            StopRoutes s = t.StopInformation(query);
            PrintStopInfo(s);
        }
    }
}

void PrintBusInfo(BusRoute r){
    if(!r.is_found){
        std::cout << "Bus "s << r.bus_name << ": not found"s << std::endl;
    }
    else{
        std::cout << "Bus "s << r.bus_name << ": " << r.stops << " stops on route, "s << r.unique_stops <<
                  " unique stops, "s << r.true_length << " route length, "s << r.curvature << " curvature"s <<std::endl;
    }
}

void PrintStopInfo(const StopRoutes& s){
    if(!s.is_found){
        std::cout << "Stop "s << s.stop_name << ": not found"s << std::endl;
    }
    else if(s.is_found && s.routes.empty()){
        std::cout << "Stop "s << s.stop_name << ": no buses"s << std::endl;
    }
    else{
        std::cout << "Stop "s << s.stop_name << ": buses"s;
        for(auto i : s.routes){
            std::cout << " "s << i;
        }
        std::cout << std::endl;
    }
}