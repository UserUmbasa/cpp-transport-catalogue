#pragma once

#include "geo.h"
#include "transport_catalogue.h"

#include <iostream>


namespace fill {

	void TransportCatalogue(std::istream& input, info_catalogue_class::TransportCatalogue&);
	info_struct::Bus ParseBus(std::string&);
	info_struct::Stop ParseStop(std::string&);
	void AddStopDistances(std::string&, info_catalogue_class::TransportCatalogue&);
}

