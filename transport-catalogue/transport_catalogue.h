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
        void AddBusRoute(const info_struct::Bus&&);
        void AddBusStop(info_struct::Stop&);
        //
        const info_struct::Bus* FindBusRoute(const std::string&) const;

        info_struct::Stop* FindBusStop(const std::string&) const;

        const info_struct::RouteInfo BusRouteInformation(const std::string&) const;
        std::set<std::string>BusToStop(const std::string&) const;

        void SetStopDistance(info_struct::Stop*, info_struct::Stop*, int);

        int GetStopDistance(const info_struct::Stop*, const info_struct::Stop*) const;

        auto ChekDistance()
        {
            return Distance;
        }

    private:
        size_t UniqueStopsCount(const std::string&) const;
        std::deque<info_struct::Bus> buses_;
        std::deque<info_struct::Stop> stops_;
        std::unordered_map<std::string_view, const info_struct::Bus*> finderbus_;
        std::unordered_map<std::string_view, info_struct::Stop*> finderstop_;
        std::unordered_map<std::string, std::set<std::string>> bustoforstop_;
        //где ключ пара остановок, значение - расстояние между ними
        std::unordered_map<std::pair<const info_struct::Stop*, const  info_struct::Stop*>, int, info_struct::DistanceHasher> Distance;
    };
}
