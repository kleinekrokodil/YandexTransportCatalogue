#pragma once

#include "transport_catalogue.h"

namespace transport_catalogue::stat_reader {
    void StatReader(TransportCatalogue t);

    void PrintBusInfo(BusRoute r);

    void PrintStopInfo(const StopRoutes &s);
}