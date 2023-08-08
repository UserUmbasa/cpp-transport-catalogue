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
  
    ////////////////////////////////////////////////////////
    struct Bus;

    struct Stop {
        std::string name;
        double latitude;
        double longitude;

        std::vector<Bus*> buses;
    };

    struct Bus {
        std::string name;
        std::vector<Stop*> stops;
        size_t route_length;
    };
    struct Distance {
        const Stop* A;
        const Stop* B;
        int distance;
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


namespace info_catalogue_class 
{
    typedef  std::unordered_map<std::string_view, info_struct::Stop*> StopMap;
    typedef  std::unordered_map<std::string_view, info_struct::Bus*> BusMap;
    typedef  std::unordered_map<std::pair<const info_struct::Stop*, const  info_struct::Stop*>, int, info_struct::DistanceHasher> DistanceMap;

    class TransportCatalogue 
    {
    public:
        void Add_Bus(info_struct::Bus&&);
        void Add_Stop(info_struct::Stop&& stop);       
        void Add_Distance(const std::vector<info_struct::Distance>& distances);

        size_t get_distance_to_bus(info_struct::Bus* bus);
        size_t get_distance_stop(const info_struct::Stop* start, const info_struct::Stop* finish);
        info_struct::Stop* get_stop(std::string_view stop_name);
        info_struct::Bus* get_bus(std::string_view bus_name);

        std::unordered_set<const info_struct::Bus*> stop_get_uniq_buses(info_struct::Stop* stop);
        std::unordered_set<const info_struct::Stop*> get_uniq_stops(info_struct::Bus* bus);
        double get_length(info_struct::Bus* bus);

    private:
        std::deque<info_struct::Bus> buses_;
        std::deque<info_struct::Stop> stops_;
        StopMap stopname_to_stop;
        DistanceMap distance_to_stop;
        BusMap busname_to_bus;

    };
}
