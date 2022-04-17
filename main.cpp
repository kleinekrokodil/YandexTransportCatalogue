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
        serializer::Path file_path = std::filesystem::path(json_input.SerializationSettingsReturn());
        serializer::SaveTransportCatalogue(file_path, t);
    } else if (mode == "process_requests"sv) {
        serializer::Path file_path = std::filesystem::path(json_input.SerializationSettingsReturn());
        transport_catalogue::TransportCatalogue t(serializer::LoadTransportCatalogue(file_path));
        //TransportRouter tr(t, json_input.RouterSettingsReturn());
        request_handler::RequestHandler answers(t, json_input.StatRequestsReturn());
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