#include "stat_reader.h"
#include <algorithm>
#include <iomanip>
namespace info_catalogue_class
{
    namespace out
    {
        namespace bus
        {
            void Show(Bus_Query_Result bus_info, std::ostream& output)
            {
                if (bus_info.not_found) {
                    output << "Bus " << bus_info.name << ": not found" << std::endl;
                }
                else {
                    output << "Bus " << bus_info.name << ": "
                        << bus_info.stops_on_route << " stops on route, "
                        << bus_info.unique_stops << " unique stops, "
                        << bus_info.route_length << " route length, "
                        << std::setprecision(6) << bus_info.curvature
                        << " curvature" << std::endl;
                }
            }
            Bus_Query_Result query(TransportCatalogue& catalogue, std::string_view bus_name)
            {
                Bus_Query_Result bus_info;
                auto entry = 4;
                bus_name = bus_name.substr(entry);

                info_struct::Bus* bus = catalogue.get_bus(bus_name);
                if (bus != nullptr) 
                {
                    bus_info.name = bus->name;
                    bus_info.not_found = false;
                    bus_info.stops_on_route = bus->stops.size();
                    bus_info.unique_stops = catalogue.get_uniq_stops(bus).size();
                    bus_info.route_length = bus->route_length;
                    bus_info.curvature = double(catalogue.get_distance_to_bus(bus) / catalogue.get_length(bus));
                }
                else {
                    bus_info.name = bus_name;
                    bus_info.not_found = true;
                }
                return bus_info;
            }
        }
        namespace stop
        {
            void Show(Stop_Query_Result stop_info, std::ostream& output)
            {
                if (stop_info.not_found) {
                    output << "Stop " << stop_info.name << ": not found" << std::endl;
                }
                else {
                    if (stop_info.buses_name.size() == 0) {
                        output << "Stop " << stop_info.name << ": no buses" << std::endl;
                    }
                    else {
                        output << "Stop " << stop_info.name << ": buses ";

                        for (std::string_view bus_name : stop_info.buses_name) {
                            output << bus_name;
                            output << " ";
                        }
                        output << std::endl;
                    }
                }
            }
            Stop_Query_Result query(TransportCatalogue& catalogue, std::string_view stop_name)
            {
                auto entry = 5;
                stop_name = stop_name.substr(entry);
                std::unordered_set<const info_struct::Bus*> unique_buses;

                Stop_Query_Result stop_info;

                info_struct::Stop* stop = catalogue.get_stop(stop_name);

                if (stop != NULL) {
                    stop_info.name = stop->name;
                    stop_info.not_found = false;
                    unique_buses = catalogue.stop_get_uniq_buses(stop);

                    if (unique_buses.size() > 0) {
                        for (const info_struct::Bus* bus : unique_buses) {
                            stop_info.buses_name.push_back(bus->name);
                        }
                        std::sort(stop_info.buses_name.begin(), stop_info.buses_name.end());
                    }
                }
                else {
                    stop_info.name = stop_name;
                    stop_info.not_found = true;
                }
                return stop_info;
            }
        }

        void Transport(std::istream& input, info_catalogue_class::TransportCatalogue& ctlg)
        {
            std::string count;
            std::getline(input, count);

            std::string str;
            std::vector<std::string> query;
            auto amount = stoi(count);

            for (int i = 0; i < amount; ++i) {
                std::getline(input, str);
                query.push_back(str);
            }

            for (auto& str : query)
            {
                Split_Search(ctlg, str);
            }
        }

        void Split_Search(info_catalogue_class::TransportCatalogue& ctlg, std::string_view str)
        {
            if (str.substr(0, 3) == "Bus")
            {
                bus::Show(bus::query(ctlg, str), std::cout);
            }
            else if (str.substr(0, 4) == "Stop") {
                stop::Show(stop::query(ctlg, str), std::cout);
            }
            else {
                std::cout << "Error query" << std::endl;
            }
        }
    }
}
