#pragma once
#include "transport_catalogue.h"
#include <iostream>

namespace out {
	void Transport(infocatalogueclass::TransportCatalogue& catalogue);
	void Bus(infocatalogueclass::TransportCatalogue&, std::string);
	void Stop(infocatalogueclass::TransportCatalogue&, std::string);
}
