#include "input_reader.h"

#include <string>
#include <vector>
#include <cmath>

void fill::TransportCatalogue(std::istream& input, info_catalogue_class::TransportCatalogue& ctlg) {

    std::string count;
    std::getline(input, count);
    auto index = stoi(count);

    std::vector<std::string> buses_;
    std::vector<std::string> stop_distances_;

    for (size_t i = 0; i < index; ++i) {
        std::string keyword, inf;
        std::cin >> keyword;
        std::getline(std::cin, inf);
        if (keyword == "Stop") 
        {
            info_struct::Stop stop = ParseStop(inf);
            ctlg.AddBusStop(stop);
            stop_distances_.push_back(inf);
        }
        if (keyword == "Bus") {
            buses_.push_back(inf);
        }
    }
    for (auto& stop_ : stop_distances_) 
    {
        AddStopDistances(stop_, ctlg);
    }
    for (auto& bus_ : buses_) 
    {
        ctlg.AddBusRoute(ParseBus(bus_));
    }
}

info_struct::Stop fill::ParseStop(std::string& inf) 
{
    info_struct::Stop stop;
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

info_struct::Bus fill::ParseBus(std::string& inf) {
    info_struct::Bus bus_info;
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

void fill::AddStopDistances(std::string& inf, info_catalogue_class::TransportCatalogue& ctlg) {
    if (!inf.empty()) 
    {
        std::string stop_from_name = ParseStop(inf).name;
        info_struct::Stop* from = ctlg.FindBusStop(stop_from_name);
        
        while (!inf.empty()) 
        {
            int distanse = 0;
            std::string stop_to_name;
            distanse = std::stoi(inf.substr(0, inf.find_first_of("m to ")));
            inf.erase(0, inf.find_first_of("m to ") + 5);
            if (inf.find("m to ") == inf.npos) 
            {
                stop_to_name = inf.substr(0, inf.npos - 1);
                info_struct::Stop* to = ctlg.FindBusStop(stop_to_name);
                ctlg.SetStopDistance(from, to, distanse);
                inf.clear();
            }
            else 
            {
                stop_to_name = inf.substr(0, inf.find_first_of(','));
                info_struct::Stop* to = ctlg.FindBusStop(stop_to_name);
                ctlg.SetStopDistance(from, to, distanse);
                //ctlg.SetStopDistance(to, from, distanse);
                /*if (!ctlg.FindBusStop(to->name)->stop_distances.count(from->name)) 
                {
                    ctlg.SetStopDistance(to, from, distanse);
                }*/
                inf.erase(0, inf.find_first_of(',') + 2);
            }
        }
    }
}
