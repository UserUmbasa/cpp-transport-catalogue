#pragma once
#include "transport_catalogue.h"
#include <iostream>
/*stat_reader.h, stat_reader.cpp — чтение запросов на вывод и сам вывод;*/
namespace out 
{
	void Transport(std::istream& input, info_catalogue_class::TransportCatalogue& catalogue);
	void Bus(info_catalogue_class::TransportCatalogue&, std::string);
	void Stop(info_catalogue_class::TransportCatalogue&, std::string);
}
