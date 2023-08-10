#pragma once

#include "geo.h"

#include <deque>
#include <string>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <set>
#include <map>
#include <functional>
#include <unordered_set>

namespace info_struct 
{
    struct Bus {
        std::string number;
        std::vector<std::string> stops;
        bool circular_route;
    };


    struct Stop {
        std::string name;
        coordinate::Coordinates coordinates;
    };

    struct RouteInfo {
        size_t stops_count;
        size_t unique_stops_count;
        double route_length;
        double curvature;
    };
    struct DistanceHasher {
        std::hash<const void*> hasher;

        std::size_t operator()(const std::pair<const Stop*, const Stop*> pair_stops) const noexcept
        {
            auto hash_1 = static_cast<const void*>(pair_stops.first);
            auto hash_2 = static_cast<const void*>(pair_stops.second);
            return hasher(hash_1) * 17 + hasher(hash_2);
        }
    };
}


namespace info_catalogue_class {
    class TransportCatalogue {
    public:
        void AddBusRoute(const info_struct::Bus& bus);
        void AddBusStop(const info_struct::Stop& stop);
        
        const info_struct::Bus* FindBusRoute(const std::string& str) const;

        info_struct::Stop* FindBusStop(const std::string& str) const;

        const info_struct::RouteInfo BusRouteInformation(const std::string& str) const;
        std::set<std::string> BusToStop(const std::string& str) const;

        void SetStopDistance(const info_struct::Stop* one , const info_struct::Stop* two, int dist);

        int GetStopDistance(const info_struct::Stop* one, const info_struct::Stop* two) const;

    private:
        size_t UniqueStopsCount(const std::string&) const;
        std::deque<info_struct::Bus> buses_;
        std::deque<info_struct::Stop> stops_;
        std::unordered_map<std::string_view, const info_struct::Bus*> finder_bus_;
        std::unordered_map<std::string_view, info_struct::Stop*> finder_stop_;
        std::unordered_map<std::string, std::set<std::string>> bus_to_for_stop_;
        //где ключ пара остановок, значение - расстояние между ними
        std::unordered_map<std::pair<const info_struct::Stop*, const  info_struct::Stop*>, int, info_struct::DistanceHasher> distance;
    };
}

