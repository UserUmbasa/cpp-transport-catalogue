#include "transport_catalogue.h"
#include <execution>
namespace info_catalogue_class
{
    void TransportCatalogue::Add_Bus(info_struct::Bus&& bus)
    {
        info_struct::Bus* bus_buf;

        buses_.push_back(std::move(bus));
        bus_buf = &buses_.back();
        busname_to_bus.insert(BusMap::value_type(bus_buf->name, bus_buf));

        for (info_struct::Stop* stop : bus_buf->stops) {
            stop->buses.push_back(bus_buf);
        }

        bus_buf->route_length = get_distance_to_bus(bus_buf);
    }
    void TransportCatalogue::Add_Stop(info_struct::Stop&& stopinfo)
    {
        stops_.push_back(std::move(stopinfo));
        info_struct::Stop* stop_buf = &stops_.back();
        stopname_to_stop.insert(StopMap::value_type(stop_buf->name, stop_buf));
    }
    void TransportCatalogue::Add_Distance(const std::vector<info_struct::Distance>& distances)
    {
        for (auto distance : distances) {
            auto dist_pair = std::make_pair(distance.A, distance.B);
            distance_to_stop.insert(DistanceMap::value_type(dist_pair, distance.distance));
        }
    }

    size_t TransportCatalogue::get_distance_to_bus(info_struct::Bus* bus)
    {
        size_t distance = 0;
        auto stops_size = bus->stops.size() - 1;
        for (int i = 0; i < stops_size; i++) {
            distance += get_distance_stop(bus->stops[i], bus->stops[i + 1]);
        }
        return distance;
    }

    size_t TransportCatalogue::get_distance_stop(const info_struct::Stop* start, const info_struct::Stop* finish)
    {
        if (distance_to_stop.empty()) {
            return 0;
        }

        try {

            auto dist_pair = std::make_pair(start, finish);
            return distance_to_stop.at(dist_pair);

        }
        catch (const std::out_of_range& e) {

            try {
                auto dist_pair = std::make_pair(finish, start);
                return distance_to_stop.at(dist_pair);
            }
            catch (const std::out_of_range& e) {
                return 0;
            }

        }
    }

    info_struct::Stop* TransportCatalogue::get_stop(std::string_view stop_name)
    {
        if (stopname_to_stop.empty()) {
            return nullptr;
        }

        try {
            return stopname_to_stop.at(stop_name);
        }
        catch (const std::out_of_range& e) {
            return nullptr;
        }
    }
    info_struct::Bus* TransportCatalogue::get_bus(std::string_view bus_name)
    {
        if (busname_to_bus.empty()) {
            return nullptr;
        }

        try {
            return busname_to_bus.at(bus_name);
        }
        catch (const std::out_of_range& e) {
            return nullptr;
        }
    }
    std::unordered_set<const info_struct::Bus*> TransportCatalogue::stop_get_uniq_buses(info_struct::Stop* stop)
    {
        std::unordered_set<const info_struct::Bus*> unique_stops;

        unique_stops.insert(stop->buses.begin(), stop->buses.end());

        return unique_stops;
    }
    std::unordered_set<const info_struct::Stop*> TransportCatalogue::get_uniq_stops(info_struct::Bus* bus)
    {
        std::unordered_set<const info_struct::Stop*> unique_stops;

        unique_stops.insert(bus->stops.begin(), bus->stops.end());

        return unique_stops;
    }
    double TransportCatalogue::get_length(info_struct::Bus* bus)
    {
        return transform_reduce(next(bus->stops.begin()),
            bus->stops.end(),
            bus->stops.begin(),
            0.0,
            std::plus<>{},
            [](const info_struct::Stop* lhs, const info_struct::Stop* rhs) {
                return coordinate::ComputeDistance({ (*lhs).latitude,
                                        (*lhs).longitude }, { (*rhs).latitude,
                                                              (*rhs).longitude });
            });
    }
}
