#include "transport_catalogue.h"
#include "transport_catalogue.pb.h"
#include "map_renderer.h"
#include "map_renderer.pb.h"
#include <filesystem>
#include <tuple>

class Serializer {
public:
    using TransportCatalogue = transport_catalogue::TransportCatalogue;
    using Path = std::filesystem::path;

    static void
    SaveTransportCatalogue(const Path &file, const TransportCatalogue &db_, const RendererSettings &renderer_settings);

    static std::tuple<TransportCatalogue, RendererSettings> DeserializeDB(const Path &file);

private:
    static transport_catalogue_serialize::RGB SerializeRGB(const svg::Rgb& rgb_color);
    static transport_catalogue_serialize::RGBA SerializeRGBA(const svg::Rgba& rgba_color);
    static transport_catalogue_serialize::Color SerializeColor(const svg::Color& color);
    static transport_catalogue_serialize::Point SerializePoint(const svg::Point& point);
    static transport_catalogue_serialize::RendererSettings SerializeRendererSettings(const RendererSettings& renderer_settings);

    static svg::Rgb DeserializeRGB(const transport_catalogue_serialize::RGB& s_rgb_color);
    static svg::Rgba DeserializeRGBA(const transport_catalogue_serialize::RGBA& s_rgba_color);
    static svg::Color DeserializeColor(const transport_catalogue_serialize::Color& s_color);
    static svg::Point DeserializePoint(const transport_catalogue_serialize::Point& s_point);
    static RendererSettings DeserializeRendererSettings(const transport_catalogue_serialize::RendererSettings& s_renderer_settings);
};