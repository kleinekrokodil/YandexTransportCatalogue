#include "json_reader.h"
#include <sstream>

namespace json_reader {

    JsonReader::JsonReader(const json::Document& document){
        if(!document.GetRoot().IsMap()){
            throw std::invalid_argument("Incorrect JSON");
        }
        const auto& query_map = document.GetRoot().AsMap();
        if(!query_map.count("base_requests") || !query_map.count("stat_requests") || !query_map.count("render_settings")){
            throw std::invalid_argument("Incorrect JSON");
        }
        JsonBaseReader(query_map.at("base_requests").AsArray());
        JsonStatReader(query_map.at("stat_requests").AsArray());
        JsonRenderSettingsReader(query_map.at("render_settings").AsMap());
    }

    const std::deque<std::string>& JsonReader::BaseRequestsReturn(){
        return base_request_;
    }
    const std::vector<std::pair<int, std::string>>& JsonReader::StatRequestsReturn(){
        return stat_request_;
    }

    const RendererSettings& JsonReader::RenderSettingsReturn(){
        return render_settings_;
    }

    void JsonReader::JsonBaseReader(const json::Array& base){
        for(const auto& query : base){
            if(!query.IsMap()){
                throw std::invalid_argument("Incorrect base requests");
            }
            if(query.AsMap().at("type").AsString() == "Stop"){
                base_request_.push_front(JsonStopToString(query.AsMap()));
            }
            if(query.AsMap().at("type").AsString() == "Bus"){
                base_request_.push_back(JsonBusToString(query.AsMap()));
            }
        }
    }

    void JsonReader::JsonStatReader(const json::Array& stat){
        for(const auto& query : stat){
            if(!query.IsMap() || !query.AsMap().count("type") || !query.AsMap().count("id")){
                throw std::invalid_argument("Incorrect stat requests");
            }
            std::string request;
            request += query.AsMap().at("type").AsString();
            if(query.AsMap().count("name")) {
                request += ' ';
                request += query.AsMap().at("name").AsString();
            }
            stat_request_.push_back({query.AsMap().at("id").AsInt(), request});
            }
    }

    void JsonReader::JsonRenderSettingsReader(const json::Dict& settings){
        render_settings_.width = settings.at("width").AsDouble();
        render_settings_.height = settings.at("height").AsDouble();
        render_settings_.padding = settings.at("padding").AsDouble();
        render_settings_.line_width = settings.at("line_width").AsDouble();
        render_settings_.stop_radius = settings.at("stop_radius").AsDouble();
        render_settings_.bus_label_font_size = settings.at("bus_label_font_size").AsInt();
        render_settings_.bus_label_offset.x = settings.at("bus_label_offset").AsArray().front().AsDouble();
        render_settings_.bus_label_offset.y = settings.at("bus_label_offset").AsArray().back().AsDouble();
        render_settings_.stop_label_font_size = settings.at("stop_label_font_size").AsInt();
        render_settings_.stop_label_offset.x = settings.at("stop_label_offset").AsArray().front().AsDouble();
        render_settings_.stop_label_offset.y = settings.at("stop_label_offset").AsArray().back().AsDouble();
        render_settings_.underlayer_color = ReadColor(settings.at("underlayer_color"));
        render_settings_.underlayer_width = settings.at("underlayer_width").AsDouble();
        for(const auto& color : settings.at("color_palette").AsArray()){
            render_settings_.color_palette.push_back(ReadColor(color));
        }
    }

    svg::Color JsonReader::ReadColor(const json::Node& node){
        if (node.IsString()) {
            return node.AsString();
        }
        else if (node.IsArray()) {
            const auto& color = node.AsArray();

            if (color.size() == 3) {
                return svg::Rgb(
                        color.at(0).AsInt(),
                        color.at(1).AsInt(),
                        color.at(2).AsInt()
                );
            }
            else if (color.size() == 4) {
                return svg::Rgba(
                        color.at(0).AsInt(),
                        color.at(1).AsInt(),
                        color.at(2).AsInt(),
                        color.at(3).AsDouble()
                );
            }
        }
        throw std::invalid_argument("Can't read color from JSON");
    }

    std::string JsonReader::JsonStopToString(const json::Dict& json_stop){
        std::stringstream result;
        result.precision(15);
        if(json_stop.at("type").AsString() == "Stop") {
            result << json_stop.at("type").AsString();
            result << ' ';
            result << json_stop.at("name").AsString();
            result << ": ";
            result << json_stop.at("latitude").AsDouble();
            result << ", ";
            result << json_stop.at("longitude").AsDouble();
            if(!json_stop.at("road_distances").AsMap().empty()) {
                result << ", ";
                bool is_first = true;
                for (const auto &next_stop: json_stop.at("road_distances").AsMap()) {
                    if(!is_first){
                        result << ", ";
                    }
                    result << next_stop.second.AsInt();
                    result << "m to ";
                    result << next_stop.first;
                    is_first = false;
                }
            }
        }
        else{
            throw std::invalid_argument("Incorrect JSON: not a stop description");
        }
        return result.str();
    }

    std::string JsonReader::JsonBusToString(const json::Dict& json_bus){
        std::string result;
        if(json_bus.at("type").AsString() == "Bus") {
            result += json_bus.at("type").AsString();
            result += ' ';
            result += json_bus.at("name").AsString();
            result += ": ";
            bool is_circle = json_bus.at("is_roundtrip").AsBool();
            bool is_first = true;
            std::string separator = is_circle ? " > " : " - ";
            for(const auto& stop : json_bus.at("stops").AsArray()){
                if(!is_first){
                    result += separator;
                }
                result += stop.AsString();
                is_first = false;
            }
        }
        else{
            throw std::invalid_argument("Incorrect JSON: not a bus description");
        }
        return result;
    }

    json::Document JsonReader::MakeJSON(const std::vector<std::pair<int, std::variant<BusRoute, StopRoutes, svg::Document>>>& answers) {
        std::stringstream str;
        str << "[";
        bool is_first = true;
        for (const auto &answer: answers) {
            if (!is_first) {
                str << ", ";
            }
            str << "{\"request_id\": ";
            str << answer.first;
            str << ", ";
            if (std::holds_alternative<BusRoute>(answer.second)) {
                const auto &bus = std::get<BusRoute>(answer.second);
                if (bus.is_found) {
                    str << "\"curvature\": ";
                    str << bus.curvature;
                    str << ", ";
                    str << "\"route_length\": ";
                    str << bus.true_length;
                    str << ", ";
                    str << "\"stop_count\": ";
                    str << bus.stops;
                    str << ", ";
                    str << "\"unique_stop_count\": ";
                    str << bus.unique_stops;
                    str << "} ";
                } else {
                    str << R"("error_message": "not found"})";
                }
            }
            if (std::holds_alternative<StopRoutes>(answer.second)) {
                const auto &stop = std::get<StopRoutes>(answer.second);
                if (stop.is_found) {
                    str << "\"buses\": [";
                    bool is_first = true;
                    for (const auto &bus: stop.routes) {
                        if (!is_first) {
                            str << ", ";
                        }
                        str << "\"";
                        str << bus;
                        str << "\"";
                        is_first = false;
                    }
                    str << "] ";
                } else {
                    str << R"("error_message": "not found")";
                }
                str << "}";
            }
            if (std::holds_alternative<svg::Document>(answer.second)) {
                const auto &svg_doc = std::get<svg::Document>(answer.second);
                std::stringstream svg_str;
                svg_doc.Render(svg_str);
                str << R"("map": ")";
                char ch;
                while (svg_str.get(ch)) {
                    if (ch == '\n') {
                        str << "\\n";
                    } else if (ch == '\r') {
                        str << "\\r";
                    } else if (ch == '\t') {
                        str << "\\t";
                    } else if (ch == '\\') {
                        str << "\\\\";
                    } else if (ch == '"') {
                        str << "\\\"";
                    } else {
                        str << ch;
                    }
                }
                str << "\"}";
            }
            is_first = false;
        }
        str << "]";
        return json::Load(str);
    }
}//namespace json_reader