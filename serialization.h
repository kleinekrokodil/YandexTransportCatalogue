#include "transport_catalogue.h"
#include "transport_catalogue.pb.h"
#include <filesystem>

namespace serializer {
    using TransportCatalogue = transport_catalogue::TransportCatalogue;
    using Path = std::filesystem::path;

    void SaveTransportCatalogue(const Path &file, const TransportCatalogue &db_);

    TransportCatalogue LoadTransportCatalogue(const Path &file);
}