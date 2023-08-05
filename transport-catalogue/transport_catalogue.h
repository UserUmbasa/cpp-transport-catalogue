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

namespace infostruct 
{
    struct Bus {
        std::string number;
        std::vector<std::string> stops;
        bool circular_route;
    };


    struct Stop {
        std::string name;
        coordinate::Coordinates coordinates;
        std::unordered_map<std::string, int> stop_distances;
    };

    struct RouteInfo {
        size_t stops_count;
        size_t unique_stops_count;
        double route_length;
        double curvature;
    };
}


namespace infocatalogueclass 
{
    class TransportCatalogue {
    public:
        void AddBusRoute(infostruct::Bus&);
        void AddBusStop(infostruct::Stop&);
        const infostruct::Bus* FindBusRoute(const std::string&) const;
        infostruct::Stop* FindBusStop(const std::string&) const;
        const infostruct::RouteInfo BusRouteInformation(const std::string&) const;
        size_t UniqueStopsCount(const std::string&) const;
        std::set<std::string>BusToStop(const std::string&) const;
        void SetStopDistance(infostruct::Stop*, infostruct::Stop*, int);
        int GetStopDistance(const infostruct::Stop*, const infostruct::Stop*) const;

    private:
        std::deque<infostruct::Bus> buses_;
        std::deque<infostruct::Stop> stops_;
        std::unordered_map<std::string_view, const infostruct::Bus*> finderbus_;
        std::unordered_map<std::string_view, infostruct::Stop*> finderstop_;
        std::unordered_map<std::string, std::set<std::string>> bustoforstop_;
    };
}
