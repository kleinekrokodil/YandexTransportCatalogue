#include "input_reader.h"
#include <iostream>

using namespace std::string_literals;

std::deque<std::string> InputReader() {
    int req_num;
    std::deque<std::string> requests;
    std::cin >> req_num;
    std::cin.ignore();
    for(int i = 0; i < req_num; ++i){
        std::string query;
        std::getline(std::cin, query);
        while (query[0] == ' '){
            query.erase(0, 1);
        }
        if(query.substr(0, 4) == "Stop"s){
            requests.push_front(query);
        }
        else if(query.substr(0, 3) == "Bus"s){
            requests.push_back(query);
        }
    }
    return requests;
}