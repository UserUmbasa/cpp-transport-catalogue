#include "input_reader.h"
#include "stat_reader.h"

// main.cpp — точка входа.
int main() {
    infocatalogueclass::TransportCatalogue catalogue;
    fill::TransportCatalogue(catalogue);
    out::Transport(catalogue);
}
