#include "serialization.h"
#include <fstream>

using namespace std;

void Serializer::SaveTransportCatalogue(const Path &file, const TransportCatalogue &db_,
                                        const RendererSettings &renderer_settings) {
    ofstream out(file, ios::binary);
    transport_catalogue_serialize::TransportCatalogue sdb;
    for(const auto& query : db_.GetBase()){
        sdb.add_queries(query);
    }
    *sdb.mutable_render_settings() = SerializeRendererSettings(renderer_settings);
    sdb.SerializeToOstream(&out);
}

tuple<transport_catalogue::TransportCatalogue, RendererSettings> Serializer::DeserializeDB(const Serializer::Path &file) {
    ifstream ifs(file, ios::binary);
    transport_catalogue_serialize::TransportCatalogue sdb;
    sdb.ParseFromIstream(&ifs);
    deque<string> result;
    for(int i = 0; i < sdb.queries_size(); ++i){
        result.push_back(sdb.queries(i));
    }
    RendererSettings renderer_settings = DeserializeRendererSettings(sdb.render_settings());
    return {TransportCatalogue(result), renderer_settings};
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

svg::Rgb Serializer::DeserializeRGB(const transport_catalogue_serialize::RGB &s_rgb_color) {
    svg::Rgb ds_rgb;
    ds_rgb.red = s_rgb_color.red();
    ds_rgb.green = s_rgb_color.green();
    ds_rgb.blue = s_rgb_color.blue();
    return ds_rgb;
}

svg::Rgba Serializer::DeserializeRGBA(const transport_catalogue_serialize::RGBA &s_rgba_color) {
    svg::Rgba ds_rgba;
    ds_rgba.red = s_rgba_color.red();
    ds_rgba.green = s_rgba_color.green();
    ds_rgba.blue = s_rgba_color.blue();
    ds_rgba.opacity = s_rgba_color.opacity();
    return ds_rgba;
}

svg::Color Serializer::DeserializeColor(const transport_catalogue_serialize::Color &s_color) {
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

svg::Point Serializer::DeserializePoint(const transport_catalogue_serialize::Point &s_point) {
    return svg::Point({s_point.x(), s_point.y()});
}

RendererSettings
Serializer::DeserializeRendererSettings(const transport_catalogue_serialize::RendererSettings &s_renderer_settings) {
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
