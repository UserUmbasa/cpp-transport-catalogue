#pragma once

#include "geo.h"
#include "transport_catalogue.h"

#include <iostream>


namespace info_catalogue_class
{
	void Transport_Catalogue(std::istream& input, TransportCatalogue& ctlg);
	info_struct::Bus Parse_Bus(std::string&, TransportCatalogue& ctlg);
	info_struct::Stop Parse_Stop(std::string&);
	std::vector<info_struct::Distance> Parse_Distances(const std::string& inf,TransportCatalogue& ctlg);

}
