#pragma once

#include "geo.h"
#include "transport_catalogue.h"

#include <iostream>


namespace fill {

	void TransportCatalogue(std::istream& input, info_catalogue_class::TransportCatalogue& ctlg);
	info_struct::Bus ParseBus(std::string& str);
	info_struct::Stop ParseStop(std::string& str);
	void AddStopDistances(std::string&, info_catalogue_class::TransportCatalogue& ctlg);
}


