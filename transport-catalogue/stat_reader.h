#pragma once
#include "transport_catalogue.h"
#include <iostream>
/*stat_reader.h, stat_reader.cpp — чтение запросов на вывод и сам вывод;*/
namespace out 
{
	void Show_data_on_request(std::istream& input, info_catalogue_class::TransportCatalogue& catalogue, std::ostream& out);
	void Show_Bus(info_catalogue_class::TransportCatalogue& ctlg, std::string str, std::ostream& out);
	void Show_Stop(info_catalogue_class::TransportCatalogue& ctlg, std::string str, std::ostream& out);
}
