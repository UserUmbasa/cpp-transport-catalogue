#include "input_reader.h"
#include "stat_reader.h"


int main() {
    infocatalogueclass::TransportCatalogue catalogue;
    fill::TransportCatalogue(catalogue);
    out::Transport(catalogue);
}
