#include "input_reader.h"
#include "stat_reader.h"


int main() {
    info_catalogue_class::TransportCatalogue catalogue;
    fill::TransportCatalogue(std::cin, catalogue);
    out::Transport(std::cin, catalogue);
}
