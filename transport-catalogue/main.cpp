#include "input_reader.h"
#include "stat_reader.h"


int main() {
    info_catalogue_class::TransportCatalogue catalogue;
    Transport_Catalogue(std::cin, catalogue);
    info_catalogue_class::out::Transport(std::cin, catalogue);
}
