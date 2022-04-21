#include "transport_catalogue.h"
#include "transport_catalogue.pb.h"
#include "map_renderer.h"
#include "map_renderer.pb.h"
#include "transport_router.h"
#include "transport_router.pb.h"
#include <filesystem>
#include <tuple>

class Serializer {
public:
    using TransportCatalogue = transport_catalogue::TransportCatalogue;
    using Path = std::filesystem::path;

    static void
    SaveTransportCatalogue(const Path &file, const TransportCatalogue &db_,
                           const RendererSettings &renderer_settings,
                           const TransportRouter &router);

private:

    //Сериализация  map_renderer
    static transport_catalogue_serialize::RGB SerializeRGB(const svg::Rgb& rgb_color);
    static transport_catalogue_serialize::RGBA SerializeRGBA(const svg::Rgba& rgba_color);
    static transport_catalogue_serialize::Color SerializeColor(const svg::Color& color);
    static transport_catalogue_serialize::Point SerializePoint(const svg::Point& point);
    static transport_catalogue_serialize::RendererSettings SerializeRendererSettings(const RendererSettings& renderer_settings);

    //Сериализация transport_router
    static transport_catalogue_serialize::Edge SerializeEdge(const graph::Edge<double>& edge);
    static transport_catalogue_serialize::IncidenceList SerializeIncidenceList(const std::vector<graph::EdgeId>& incidence_list);
    static transport_catalogue_serialize::Graph SerializeGraph(const graph::DirectedWeightedGraph<double>& graph);
    static transport_catalogue_serialize::RouterSettings SerializeRouterSettings(const RouterSettings& settings);
    static transport_catalogue_serialize::BusTripEdges SerializeBusTripEdges(const BusTripEdges& edges);
    static transport_catalogue_serialize::EdgeId SerializeEdgeId(const std::pair<uint32_t, BusTripEdges>& edges_id);
    static transport_catalogue_serialize::EdgeIdMap SerializeEdgeIdMap (const std::unordered_map<uint32_t, BusTripEdges>& edges_ids);
    static transport_catalogue_serialize::StopId SerializeStopId (const std::pair<std::string_view, uint32_t>& stop_id);
    static transport_catalogue_serialize::StopIdMap SerializeStopIdMap (const std::unordered_map<std::string_view, uint32_t>& stop_ids);
    static transport_catalogue_serialize::TransportRouter SerializeTransportRouter(const TransportRouter& router);
};

class Deserializer{
public:
    Deserializer(const Serializer::Path &file);

    std::deque<std::string> GetQueries();
    RendererSettings GetRendererSettings();
    RouterSettings GetRouterSettings();
    std::unordered_map<std::string_view, uint32_t> GetStopIds();
    graph::DirectedWeightedGraph<double> GetGraph();
    std::unordered_map<uint32_t, BusTripEdges> GetEdgeIds();

private:
    std::deque<std::string> queries_for_tc_;
    RendererSettings renderer_settings_;
    RouterSettings router_settings_;
    std::unordered_map<std::string_view, uint32_t> stop_ids_;
    graph::DirectedWeightedGraph<double> graph_;
    std::unordered_map<uint32_t, BusTripEdges> edges_ids_;

    //Десериализация map_renderer
    static svg::Rgb DeserializeRGB(const transport_catalogue_serialize::RGB& s_rgb_color);
    static svg::Rgba DeserializeRGBA(const transport_catalogue_serialize::RGBA& s_rgba_color);
    static svg::Color DeserializeColor(const transport_catalogue_serialize::Color& s_color);
    static svg::Point DeserializePoint(const transport_catalogue_serialize::Point& s_point);
    static RendererSettings DeserializeRendererSettings(const transport_catalogue_serialize::RendererSettings& s_renderer_settings);
    //Десериализация transport_router
    static graph::Edge<double> DeserializeEdge(const transport_catalogue_serialize::Edge& edge);
    static std::vector<graph::EdgeId> DeserializeIncidenceList(const transport_catalogue_serialize::IncidenceList& incidence_list);
    static graph::DirectedWeightedGraph<double> DeserializeGraph(const transport_catalogue_serialize::Graph& graph);
    static RouterSettings DeserializeRouterSettings(const transport_catalogue_serialize::RouterSettings& settings);
    static BusTripEdges DeserializeBusTripEdges(const transport_catalogue_serialize::BusTripEdges& edges);
    static std::pair<uint32_t, BusTripEdges> DeserializeEdgeId(const transport_catalogue_serialize::EdgeId& edges_id);
    static std::unordered_map<uint32_t, BusTripEdges> DeserializeEdgeIdMap (const transport_catalogue_serialize::EdgeIdMap& edges_ids);
    static std::pair<std::string_view, uint32_t> DeserializeStopId (const transport_catalogue_serialize::StopId& stop_id);
    static std::unordered_map<std::string_view, uint32_t> DeserializeStopIdMap (const transport_catalogue_serialize::StopIdMap& stop_ids);
};