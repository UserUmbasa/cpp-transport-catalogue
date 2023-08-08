#include "input_reader.h"

#include <string>
#include <vector>
#include <cmath>

namespace info_catalogue_class
{
    void Transport_Catalogue(std::istream& input,TransportCatalogue& ctlg)
    {
        std::string count;
        std::getline(input, count);

        std::string str;
        std::vector<std::string> buses_;           //автобусы
        std::vector<std::string> stops_;           //остановки

        int amount = stoi(count);
        auto bus_distance = 3;

        for (int i = 0; i < amount; ++i) {
            std::getline(input, str);

            if (str != "") {
                auto space_pos = str.find_first_not_of(' ');
                str = str.substr(space_pos);

                if (str.substr(0, bus_distance) != "Bus") {
                    stops_.push_back(str);
                }
                else {
                    buses_.push_back(str);
                }
            }
        }

        for (auto& stop_ : stops_)
        {
            ctlg.Add_Stop(Parse_Stop(stop_));
        }
        for (auto& stop_ : stops_) {
            ctlg.Add_Distance(Parse_Distances(stop_, ctlg));
        }
        for (auto& bus_ : buses_) {
            ctlg.Add_Bus(Parse_Bus(bus_, ctlg));
        }
    }

    info_struct::Stop Parse_Stop(std::string& inf)
    {
        auto twopoint_pos = inf.find(':');
        auto comma_pos = inf.find(',');
        auto entry_length = 5;
        auto distance = 2;
        info_struct::Stop stop;

        stop.name = inf.substr(entry_length,
            twopoint_pos - entry_length);
        stop.latitude = stod(inf.substr(twopoint_pos + distance,
            comma_pos - twopoint_pos - distance));
        stop.longitude = stod(inf.substr(comma_pos + distance));

        return stop;
    }


    info_struct::Bus Parse_Bus(std::string& inf, TransportCatalogue& ctlg)
    {
        
            auto entry_length = 4;
            auto distance = 2;
            auto twopoint_pos = inf.find(':');
            info_struct::Bus bus;
            bus.name = inf.substr(entry_length,
                twopoint_pos - entry_length);

            inf = inf.substr(twopoint_pos + distance);

            auto more_pos = inf.find('>');
            if (more_pos == std::string_view::npos) {
                auto tire_pos = inf.find('-');

                while (tire_pos != std::string_view::npos) {
                    bus.stops.push_back(ctlg.get_stop(inf.substr(0, tire_pos - 1)));

                    inf = inf.substr(tire_pos + distance);
                    tire_pos = inf.find('-');
                }

                bus.stops.push_back(ctlg.get_stop(inf.substr(0, tire_pos - 1)));
                size_t size = bus.stops.size() - 1;

                for (size_t i = size; i > 0; i--) {
                    bus.stops.push_back(bus.stops[i - 1]);
                }

            }
            else {
                while (more_pos != std::string_view::npos) {
                    bus.stops.push_back(ctlg.get_stop(inf.substr(0, more_pos - 1)));

                    inf = inf.substr(more_pos + distance);
                    more_pos = inf.find('>');
                }

                bus.stops.push_back(ctlg.get_stop(inf.substr(0, more_pos - 1)));
            }

            return bus;
    }

    std::vector<info_struct::Distance>Parse_Distances(const std::string& inf, TransportCatalogue& ctlg)
    {
        std::vector<info_struct::Distance> distances;
        auto entry_length = 5;
        auto twopoint_pos = inf.find(':');
        auto space = 2;

        std::string str_copy = inf;
        std::string name = str_copy.substr(entry_length,
            twopoint_pos - entry_length);
        str_copy = str_copy.substr(str_copy.find(',') + 1);
        str_copy = str_copy.substr(str_copy.find(',') + space);

        while (str_copy.find(',') != std::string::npos) {
            int distance = stoi(str_copy.substr(0, str_copy.find('m')));
            std::string stop_dist_name = str_copy.substr(str_copy.find('m') + entry_length);
            stop_dist_name = stop_dist_name.substr(0, stop_dist_name.find(','));
            distances.push_back({ ctlg.get_stop(name),
                                  ctlg.get_stop(stop_dist_name), distance });
            str_copy = str_copy.substr(str_copy.find(',') + space);
        }
        std::string last_name = str_copy.substr(str_copy.find('m') + entry_length);
        int distance = stoi(str_copy.substr(0, str_copy.find('m')));
        distances.push_back({ ctlg.get_stop(name), ctlg.get_stop(last_name), distance });

        return distances;
    }
}
