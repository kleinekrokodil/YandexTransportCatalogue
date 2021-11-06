#include "geo.h"
#include "input_reader.h"
#include "transport_catalogue.h"
#include "stat_reader.h"

int main(){
    TransportCatalogue t(InputReader());
    StatReader(t);
    return 0;
}