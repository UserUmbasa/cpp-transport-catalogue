#include "input_reader.h"

#include <string>
#include <vector>
#include <cmath>

void fill::TransportCatalogue(infocatalogueclass::TransportCatalogue& ctlg) {
    size_t count;

    std::vector<std::string> buses_;           
    std::vector<std::string> stops_;           
    std::vector<std::string> stop_distances_;  

    std::cin >> count;
    for (size_t i = 0; i < count; ++i) {
        std::string keyword, inf;
        std::cin >> keyword;
        std::getline(std::cin, inf);
        if (keyword == "Stop") {
            stops_.push_back(inf);
        }
        if (keyword == "Bus") {
            buses_.push_back(inf);
        }
    }
    stop_distances_ = stops_;
    for (auto& stop_ : stops_) 
    {
        infostruct::Stop stop = ParseStop(stop_);
        ctlg.AddBusStop(stop);
    }
    for (auto& stop_ : stop_distances_) {
        AddStopDistances(stop_, ctlg);
    }
    for (auto& bus_ : buses_) {
        infostruct::Bus bus = ParseBus(bus_);
        ctlg.AddBusRoute(bus);
        bus = {};
    }
}

infostruct::Stop fill::ParseStop(std::string& inf) {
    infostruct::Stop stop;
    std::string stop_name = inf.substr(1, inf.find_first_of(':') - inf.find_first_of(' ') - 1);

    double latitude = std::stod(inf.substr(inf.find_first_of(':') + 2, inf.find_first_of(',') - 1));
    double longitude;
    inf.erase(0, inf.find_first_of(',') + 2);
    if (inf.find_last_of(',') == inf.npos) {
        longitude = std::stod(inf.substr(0, inf.npos - 1));
        inf.clear();
    }
    else {
        longitude = std::stod(inf.substr(0, inf.find_first_of(',')));
        inf.erase(0, inf.find_first_of(',') + 2);
    }
    coordinate::Coordinates stop_coordinates = { latitude, longitude };

    stop.name = stop_name;
    stop.coordinates = stop_coordinates;

    return stop;
}

infostruct::Bus fill::ParseBus(std::string& inf) {
    infostruct::Bus bus_info;
    std::vector<std::string> route_stops;
    std::string route_number = inf.substr(1, inf.find_first_of(':') - 1);
    inf.erase(0, inf.find_first_of(':') + 2);
    bool circular_route = false;
    std::string stop_name;
    auto pos = inf.find('>') != inf.npos ? '>' : '-';
    while (inf.find(pos) != inf.npos) {
        stop_name = inf.substr(0, inf.find_first_of(pos) - 1);
        route_stops.push_back(stop_name);
        inf.erase(0, inf.find_first_of(pos) + 2);
    }
    stop_name = inf.substr(0, inf.npos - 1);
    route_stops.push_back(stop_name);
    if (pos == '>') circular_route = true;

    bus_info.circular_route = circular_route;
    bus_info.number = route_number;
    bus_info.stops = route_stops;
    return bus_info;
}

void fill::AddStopDistances(std::string& inf, infocatalogueclass::TransportCatalogue& ctlg) {
    if (!inf.empty()) {
        std::string stop_from_name = ParseStop(inf).name;
        infostruct::Stop* from = ctlg.FindBusStop(stop_from_name);

        while (!inf.empty()) {
            int distanse = 0;
            std::string stop_to_name;
            distanse = std::stoi(inf.substr(0, inf.find_first_of("m to ")));
            inf.erase(0, inf.find_first_of("m to ") + 5);
            if (inf.find("m to ") == inf.npos) {
                stop_to_name = inf.substr(0, inf.npos - 1);
                infostruct::Stop* to = ctlg.FindBusStop(stop_to_name);
                ctlg.SetStopDistance(from, to, distanse);
                if (!ctlg.FindBusStop(to->name)->stop_distances.count(from->name)) {
                    ctlg.SetStopDistance(to, from, distanse);
                }
                inf.clear();
            }
            else {
                stop_to_name = inf.substr(0, inf.find_first_of(','));
                infostruct::Stop* to = ctlg.FindBusStop(stop_to_name);
                ctlg.SetStopDistance(from, to, distanse);
                if (!ctlg.FindBusStop(to->name)->stop_distances.count(from->name)) {
                    ctlg.SetStopDistance(to, from, distanse);
                }
                inf.erase(0, inf.find_first_of(',') + 2);
            }
        }
    }
}
