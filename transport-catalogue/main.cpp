#include "input_reader.h"
#include "stat_reader.h"

int main() {
    info_catalogue_class::TransportCatalogue catalogue;
    fill::TransportCatalogue(std::cin, catalogue);
    out::Show_data_on_request(std::cin, catalogue, std::cout);
}
