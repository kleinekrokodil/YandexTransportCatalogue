#include "request_handler.h"

#include <fstream>
#include <iostream>
#include <string_view>
#include "serialization.h"

using namespace std::literals;

void PrintUsage(std::ostream& stream = std::cerr) {
    stream << "Usage: transport_catalogue [make_base|process_requests]\n"sv;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        PrintUsage();
        return 1;
    }

    const std::string_view mode(argv[1]);

    json_reader::JsonReader json_input(json::Load(std::cin));

    if (mode == "make_base"sv) {
        transport_catalogue::TransportCatalogue t(json_input.BaseRequestsReturn());
        Serializer::Path file_path = std::filesystem::path(json_input.SerializationSettingsReturn());
        Serializer::SaveTransportCatalogue(file_path, t, json_input.RenderSettingsReturn());
    } else if (mode == "process_requests"sv) {
        Serializer::Path file_path = std::filesystem::path(json_input.SerializationSettingsReturn());
        auto deserializedDb = Serializer::DeserializeDB(file_path);
        //TransportRouter tr(deserializeDb, json_input.RouterSettingsReturn());
        request_handler::RequestHandler answers(std::get<0>(deserializedDb), json_input.StatRequestsReturn(), std::get<1>(deserializedDb));
        auto answers_map = json_input.MakeJSON(answers.GetAnswers());
        Print(answers_map, std::cout);
    } else {
        PrintUsage();
        return 1;
    }
}

/*int main(){
    using namespace transport_catalogue;
    json_reader::JsonReader json_input(json::Load(std::cin));
    TransportCatalogue t(json_input.BaseRequestsReturn());
    TransportRouter tr(t, json_input.RouterSettingsReturn());
    request_handler::RequestHandler answers(t, json_input.StatRequestsReturn(), json_input.RenderSettingsReturn(), tr);
    auto answers_map = json_input.MakeJSON(answers.GetAnswers());
    Print(answers_map, std::cout);
    return 0;
}*/