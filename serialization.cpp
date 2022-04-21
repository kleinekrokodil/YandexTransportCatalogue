#include "serialization.h"
#include <fstream>

using namespace std;

void Serializer::SaveTransportCatalogue(const Path &file, const TransportCatalogue &db_,
                                        const RendererSettings &renderer_settings,
                                        const TransportRouter &router) {
    ofstream out(file, ios::binary);
    transport_catalogue_serialize::TransportCatalogue sdb;
    for(const auto& query : db_.GetBase()){
        sdb.add_queries(query);
    }
    *sdb.mutable_render_settings() = SerializeRendererSettings(renderer_settings);
    *sdb.mutable_transport_router() = SerializeTransportRouter(router);
    sdb.SerializeToOstream(&out);
}

transport_catalogue_serialize::RGB Serializer::SerializeRGB(const svg::Rgb &rgb_color) {
    transport_catalogue_serialize::RGB s_rgb;
    s_rgb.set_red(rgb_color.red);
    s_rgb.set_green(rgb_color.green);
    s_rgb.set_blue(rgb_color.blue);
    return s_rgb;
}

transport_catalogue_serialize::RGBA Serializer::SerializeRGBA(const svg::Rgba &rgba_color) {
    transport_catalogue_serialize::RGBA s_rgba;
    s_rgba.set_red(rgba_color.red);
    s_rgba.set_green(rgba_color.green);
    s_rgba.set_blue(rgba_color.blue);
    s_rgba.set_opacity(rgba_color.opacity);
    return s_rgba;
}

transport_catalogue_serialize::Color Serializer::SerializeColor(const svg::Color &color) {
    transport_catalogue_serialize::Color s_color;
    if(holds_alternative<std::string>(color)){
        s_color.set_string_color(std::get<std::string>(color));
    }
    if(holds_alternative<svg::Rgb>(color)){
        *s_color.mutable_rgb_color() = SerializeRGB(std::get<svg::Rgb>(color));
    }
    if(holds_alternative<svg::Rgba>(color)){
        *s_color.mutable_rgba_color() = SerializeRGBA(std::get<svg::Rgba>(color));
    }
    return s_color;
}

transport_catalogue_serialize::Point Serializer::SerializePoint(const svg::Point &point) {
    transport_catalogue_serialize::Point s_point;
    s_point.set_x(point.x);
    s_point.set_y(point.y);
    return s_point;
}

transport_catalogue_serialize::RendererSettings Serializer::SerializeRendererSettings(const RendererSettings &renderer_settings) {
    transport_catalogue_serialize::RendererSettings s_renderer_settings;
    s_renderer_settings.set_width(renderer_settings.width);
    s_renderer_settings.set_height(renderer_settings.height);
    s_renderer_settings.set_padding(renderer_settings.padding);
    s_renderer_settings.set_line_width(renderer_settings.line_width);
    s_renderer_settings.set_stop_radius(renderer_settings.stop_radius);
    s_renderer_settings.set_stop_label_font_size(renderer_settings.stop_label_font_size);
    s_renderer_settings.set_bus_label_font_size(renderer_settings.bus_label_font_size);
    *s_renderer_settings.mutable_bus_label_offset() = SerializePoint(renderer_settings.bus_label_offset);
    *s_renderer_settings.mutable_stop_label_offset() = SerializePoint(renderer_settings.stop_label_offset);
    *s_renderer_settings.mutable_underlayer_color() = SerializeColor(renderer_settings.underlayer_color);
    s_renderer_settings.set_underlayer_width(renderer_settings.underlayer_width);
    for(const auto & i : renderer_settings.color_palette){
        *s_renderer_settings.add_color_palette() = SerializeColor(i);
    }
    return s_renderer_settings;
}

transport_catalogue_serialize::Edge Serializer::SerializeEdge(const graph::Edge<double> &edge) {
    transport_catalogue_serialize::Edge s_edge;
    s_edge.set_from(edge.from);
    s_edge.set_to(edge.to);
    s_edge.set_weight(edge.weight);
    return s_edge;
}

transport_catalogue_serialize::IncidenceList
Serializer::SerializeIncidenceList(const std::vector<graph::EdgeId> &incidence_list) {
    transport_catalogue_serialize::IncidenceList result;
    for(const auto& i : incidence_list){
        result.add_edgeid(i);
    }
    return result;
}

transport_catalogue_serialize::Graph Serializer::SerializeGraph(const graph::DirectedWeightedGraph<double>& graph){
    transport_catalogue_serialize::Graph s_graph;
    for(const auto& edge : graph.GetEdges()){
        *s_graph.add_edges() = SerializeEdge(edge);
    }
    for(const auto& i : graph.GetIncidenceLists()){
        *s_graph.add_incidence_lists() = SerializeIncidenceList(i);
    }
    return s_graph;
}

transport_catalogue_serialize::RouterSettings Serializer::SerializeRouterSettings(const RouterSettings &settings) {
    transport_catalogue_serialize::RouterSettings s_settings;
    s_settings.set_bus_velocity(settings.bus_velocity_);
    s_settings.set_bus_wait_time(settings.bus_wait_time_);
    return s_settings;
}

transport_catalogue_serialize::BusTripEdges Serializer::SerializeBusTripEdges(const BusTripEdges &edges) {
    transport_catalogue_serialize::BusTripEdges s_edges;
    s_edges.set_bus_name({edges.bus_name_.data(), edges.bus_name_.size()});
    s_edges.set_span_count(edges.span_count_);
    s_edges.set_time(edges.time_);
    s_edges.add_stops(edges.stops_.first.data(), edges.stops_.first.size());
    s_edges.add_stops(edges.stops_.second.data(), edges.stops_.second.size());
    return s_edges;
}

transport_catalogue_serialize::EdgeId Serializer::SerializeEdgeId(const pair<uint32_t, BusTripEdges> &edges_id) {
    transport_catalogue_serialize::EdgeId s_edge_id;
    s_edge_id.set_id(edges_id.first);
    *s_edge_id.mutable_edge_info() = SerializeBusTripEdges(edges_id.second);
    return s_edge_id;
}
transport_catalogue_serialize::EdgeIdMap
Serializer::SerializeEdgeIdMap(const unordered_map<uint32_t, BusTripEdges> &edges_ids) {
    transport_catalogue_serialize::EdgeIdMap s_edges_ids;
    for(const auto& edge :edges_ids){
        *s_edges_ids.add_edges_ids() = SerializeEdgeId(edge);
    }
    return s_edges_ids;
}

transport_catalogue_serialize::StopId Serializer::SerializeStopId(const std::pair<std::string_view, uint32_t>& stop_id) {
    transport_catalogue_serialize::StopId s_stop_id;
    s_stop_id.set_stop({stop_id.first.data(), stop_id.first.size()});
    s_stop_id.set_id(stop_id.second);
    return s_stop_id;
}

transport_catalogue_serialize::StopIdMap
Serializer::SerializeStopIdMap(const unordered_map<std::string_view, uint32_t> &stop_ids) {
    transport_catalogue_serialize::StopIdMap s_stop_ids;
    for(const auto& stop : stop_ids){
        *s_stop_ids.add_stop_ids() = SerializeStopId(stop);
    }
    return s_stop_ids;
}

transport_catalogue_serialize::TransportRouter Serializer::SerializeTransportRouter(const TransportRouter &router) {
    transport_catalogue_serialize::TransportRouter s_router;
    *s_router.mutable_router_settings() = SerializeRouterSettings(router.GetRouterSettings());
    *s_router.mutable_stop_ids() = SerializeStopIdMap(router.GetStopIds());
    *s_router.mutable_edge_ids() = SerializeEdgeIdMap(router.GetEdgesIds());
    *s_router.mutable_graph() = SerializeGraph(router.GetGraph());
    return s_router;
}


Deserializer::Deserializer(const Serializer::Path &file) {
    ifstream ifs(file, ios::binary);
    transport_catalogue_serialize::TransportCatalogue sdb;
    sdb.ParseFromIstream(&ifs);

    for(int i = 0; i < sdb.queries_size(); ++i){
        queries_for_tc_.push_back(sdb.queries(i));
    }
    renderer_settings_ = DeserializeRendererSettings(sdb.render_settings());
    router_settings_ = DeserializeRouterSettings(sdb.transport_router().router_settings());
    stop_ids_ = DeserializeStopIdMap(sdb.transport_router().stop_ids());
    graph_ = DeserializeGraph(sdb.transport_router().graph());
    edges_ids_ = DeserializeEdgeIdMap(sdb.transport_router().edge_ids());
}

svg::Rgb Deserializer::DeserializeRGB(const transport_catalogue_serialize::RGB &s_rgb_color) {
    svg::Rgb ds_rgb;
    ds_rgb.red = s_rgb_color.red();
    ds_rgb.green = s_rgb_color.green();
    ds_rgb.blue = s_rgb_color.blue();
    return ds_rgb;
}

svg::Rgba Deserializer::DeserializeRGBA(const transport_catalogue_serialize::RGBA &s_rgba_color) {
    svg::Rgba ds_rgba;
    ds_rgba.red = s_rgba_color.red();
    ds_rgba.green = s_rgba_color.green();
    ds_rgba.blue = s_rgba_color.blue();
    ds_rgba.opacity = s_rgba_color.opacity();
    return ds_rgba;
}

svg::Color Deserializer::DeserializeColor(const transport_catalogue_serialize::Color &s_color) {
    svg::Color ds_color;
    switch (s_color.variant_case()) {
        case transport_catalogue_serialize::Color::kStringColor:
            ds_color = s_color.string_color();
            break;
        case transport_catalogue_serialize::Color::kRGBColor:
            ds_color = DeserializeRGB(s_color.rgb_color());
            break;
        case transport_catalogue_serialize::Color::kRGBAColor:
            ds_color = DeserializeRGBA(s_color.rgba_color());
            break;
        case transport_catalogue_serialize::Color::VARIANT_NOT_SET:
            ds_color = svg::NoneColor;
            break;
    }
    return ds_color;
}

svg::Point Deserializer::DeserializePoint(const transport_catalogue_serialize::Point &s_point) {
    return svg::Point({s_point.x(), s_point.y()});
}

RendererSettings
Deserializer::DeserializeRendererSettings(const transport_catalogue_serialize::RendererSettings &s_renderer_settings) {
    RendererSettings ds_renderer_settings;
    ds_renderer_settings.width = s_renderer_settings.width();
    ds_renderer_settings.height = s_renderer_settings.height();
    ds_renderer_settings.padding = s_renderer_settings.padding();
    ds_renderer_settings.line_width = s_renderer_settings.line_width();
    ds_renderer_settings.stop_radius = s_renderer_settings.stop_radius();
    ds_renderer_settings.stop_label_font_size = s_renderer_settings.stop_label_font_size();
    ds_renderer_settings.bus_label_font_size = s_renderer_settings.bus_label_font_size();
    ds_renderer_settings.stop_label_offset = DeserializePoint(s_renderer_settings.stop_label_offset());
    ds_renderer_settings.bus_label_offset = DeserializePoint(s_renderer_settings.bus_label_offset());
    ds_renderer_settings.underlayer_color = DeserializeColor(s_renderer_settings.underlayer_color());
    ds_renderer_settings.underlayer_width = s_renderer_settings.underlayer_width();
    for(int i = 0; i < s_renderer_settings.color_palette_size(); ++i) {
        ds_renderer_settings.color_palette.push_back(DeserializeColor(s_renderer_settings.color_palette(i)));
    }
    return ds_renderer_settings;
}

graph::Edge<double> Deserializer::DeserializeEdge(const transport_catalogue_serialize::Edge &edge) {
    graph::Edge<double> d_edge;
    d_edge.from = edge.from();
    d_edge.to = edge.to();
    d_edge.weight = edge.weight();
    return d_edge;
}

std::vector<graph::EdgeId>
Deserializer::DeserializeIncidenceList(const transport_catalogue_serialize::IncidenceList &incidence_list) {
    std::vector<graph::EdgeId> d_incidence_list;
    for(int i = 0; i < incidence_list.edgeid_size(); ++i){
        d_incidence_list.push_back(incidence_list.edgeid(i));
    }
    return d_incidence_list;
}

graph::DirectedWeightedGraph<double> Deserializer::DeserializeGraph(const transport_catalogue_serialize::Graph& graph) {
    std::vector<graph::Edge<double >> d_edges;
    for(int i = 0; i < graph.edges_size(); ++i){
        d_edges.push_back(DeserializeEdge(graph.edges(i)));
    }
    std::vector<std::vector<graph::EdgeId>> d_incidence_lists;
    for(int i = 0; i < graph.incidence_lists_size(); ++i){
        d_incidence_lists.push_back(DeserializeIncidenceList(graph.incidence_lists(i)));
    }
    return {d_edges, d_incidence_lists};
}

RouterSettings Deserializer::DeserializeRouterSettings(const transport_catalogue_serialize::RouterSettings &settings) {
    return {settings.bus_wait_time(), settings.bus_velocity()};
}

BusTripEdges Deserializer::DeserializeBusTripEdges(const transport_catalogue_serialize::BusTripEdges &edges) {
    return {edges.bus_name(), edges.time(), edges.span_count(), {edges.stops(0), edges.stops(1)}};
}

std::pair<uint32_t, BusTripEdges> Deserializer::DeserializeEdgeId(const transport_catalogue_serialize::EdgeId &edges_id) {
    return {edges_id.id(), DeserializeBusTripEdges(edges_id.edge_info())};
}

std::unordered_map<uint32_t, BusTripEdges>
Deserializer::DeserializeEdgeIdMap(const transport_catalogue_serialize::EdgeIdMap &edges_ids) {
    std::unordered_map<uint32_t, BusTripEdges> d_edges_ids;
    for(int i = 0; i < edges_ids.edges_ids_size(); ++i){
        d_edges_ids.insert(DeserializeEdgeId(edges_ids.edges_ids(i)));
    }
    return d_edges_ids;
}

std::pair<std::string_view, uint32_t>
Deserializer::DeserializeStopId(const transport_catalogue_serialize::StopId &stop_id) {
    return {stop_id.stop(), stop_id.id()};
}

std::unordered_map<std::string_view, uint32_t>
Deserializer::DeserializeStopIdMap(const transport_catalogue_serialize::StopIdMap &stop_ids) {
    std::unordered_map<std::string_view, uint32_t> d_stop_ids;
    for(int i = 0; i < stop_ids.stop_ids_size(); ++i){
        d_stop_ids.insert(DeserializeStopId(stop_ids.stop_ids(i)));
    }
    return d_stop_ids;
}

std::deque<std::string> Deserializer::GetQueries() {
    return queries_for_tc_;
}

RendererSettings Deserializer::GetRendererSettings() {
    return renderer_settings_;
}

RouterSettings Deserializer::GetRouterSettings() {
    return router_settings_;
}

std::unordered_map<std::string_view, uint32_t> Deserializer::GetStopIds() {
    return stop_ids_;
}

graph::DirectedWeightedGraph<double> Deserializer::GetGraph() {
    return graph_;
}

std::unordered_map<uint32_t, BusTripEdges> Deserializer::GetEdgeIds() {
    return edges_ids_;
}





