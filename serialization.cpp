#include "serialization.h"
#include <fstream>

using namespace std;

void serializer::SaveTransportCatalogue(const Path &file, const TransportCatalogue &db_) {
    ofstream out(file, ios::binary);
    transport_catalogue_serialize::TransportCatalogue sdb;
    for(const auto& query : db_.GetBase()){
        sdb.add_queries(query);
    }
    sdb.SerializeToOstream(&out);
}

transport_catalogue::TransportCatalogue serializer::LoadTransportCatalogue(const serializer::Path &file) {
    ifstream ifs(file, ios::binary);
    transport_catalogue_serialize::TransportCatalogue sdb;
    sdb.ParseFromIstream(&ifs);
    deque<string> result;
    for(int i = 0; i < sdb.queries_size(); ++i){
        result.push_back(sdb.queries(i));
    }
    return TransportCatalogue(result);
}
