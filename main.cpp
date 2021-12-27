#include "geo.h"
#include "transport_catalogue.h"
#include "request_handler.h"
#include "json_reader.h"


int main(){
    using namespace transport_catalogue;
    json_reader::JsonReader json_input(json::Load(std::cin));
    TransportCatalogue t(json_input.BaseRequestsReturn());
    request_handler::RequestHandler answers(t, json_input.StatRequestsReturn());
    auto answers_map = json_input.MakeJSON(answers.GetAnswers());
    Print(answers_map, std::cout);
    return 0;
}