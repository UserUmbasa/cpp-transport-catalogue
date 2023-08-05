#pragma once

#include "geo.h"
#include "transport_catalogue.h"

#include <iostream>



namespace fill {

	void TransportCatalogue(infocatalogueclass::TransportCatalogue&);
	infostruct::Bus ParseBus(std::string&);
	infostruct::Stop ParseStop(std::string&);
	void AddStopDistances(std::string&, infocatalogueclass::TransportCatalogue&);
}
