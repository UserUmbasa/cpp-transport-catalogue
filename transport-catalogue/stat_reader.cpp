#include "stat_reader.h"

#include <iomanip>

void out::Transport(infocatalogueclass::TransportCatalogue& catalogue) {
    size_t count;
    std::cin >> count;
    for (size_t i = 0; i < count; ++i) {
        std::string keyword, inf;
        std::cin >> keyword;
        std::getline(std::cin, inf);
        if (keyword == "Bus") {
            out::Bus(catalogue, inf);
        }
        if (keyword == "Stop") {
            out::Stop(catalogue, inf);
        }
    }
}

void out::Bus(infocatalogueclass::TransportCatalogue& ctlg, std::string inf) {
    std::string number = inf.substr(1, inf.npos);
    if (ctlg.FindBusRoute(number)) {
        std::cout << "Bus " << number << ": " << ctlg.BusRouteInformation(number).stops_count << " stops on route, "
            << ctlg.BusRouteInformation(number).unique_stops_count << " unique stops, " << ctlg.BusRouteInformation(number).route_length << " route length, " << ctlg.BusRouteInformation(number).curvature << " curvature\n";
    }
    else {
        std::cout << "Bus " << number << ": not found\n";
    }
}


void out::Stop(infocatalogueclass::TransportCatalogue& ctlg, std::string inf) {
    std::string stop_name = inf.substr(1, inf.npos);
    if (ctlg.FindBusStop(stop_name)) {
        std::cout << "Stop " << stop_name << ": ";
        std::set<std::string> buses = ctlg.BusToStop(stop_name);
        if (!buses.empty()) {
            std::cout << "buses ";
            for (const auto& bus : buses) {
                std::cout << bus << " ";
            }
            std::cout << "\n";
        }
        else {
            std::cout << "no buses\n";
        }
    }
    else {
        std::cout << "Stop " << stop_name << ": not found\n";
    }
}
