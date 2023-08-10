#include "stat_reader.h"

#include <iomanip>

void out::Show_data_on_request(std::istream& input, info_catalogue_class::TransportCatalogue& catalogue, std::ostream& out) {
    std::string count;
    std::getline(input, count);
    auto index = stoi(count);
    for (size_t i = 0; i < index; ++i) {
        std::string keyword, inf;
        std::cin >> keyword;
        std::getline(std::cin, inf);
        if (keyword == "Bus") {
            out::Show_Bus(catalogue, inf, out);
        }
        if (keyword == "Stop") {
            out::Show_Stop(catalogue, inf, out);
        }
    }
}

void out::Show_Bus(info_catalogue_class::TransportCatalogue& ctlg, std::string inf, std::ostream& out)
{
    std::string number = inf.substr(1, inf.npos); 
    if (ctlg.FindBusRoute(number))    
    {
        auto check = ctlg.BusRouteInformation(number);
            out << "Bus " << number << ": " 
            << check.stops_count << " stops on route, "
            << check.unique_stops_count << " unique stops, "
            << check.route_length << " route length, "
            << check.curvature << " curvature\n";
    }
    else {
           out << "Bus " << number << ": not found\n";
    }
}


void out::Show_Stop(info_catalogue_class::TransportCatalogue& ctlg, std::string inf, std::ostream& out) {
    std::string stop_name = inf.substr(1, inf.npos);
    if (ctlg.FindBusStop(stop_name)) {
        out << "Stop " << stop_name << ": ";
        std::set<std::string> buses = ctlg.BusToStop(stop_name);
        if (!buses.empty()) {
            out << "buses ";
            for (const auto& bus : buses) {
                out << bus << " ";
            }
            out << "\n";
        }
        else {
            out << "no buses\n";
        }
    }
    else {
        out << "Stop " << stop_name << ": not found\n";
    }
}
