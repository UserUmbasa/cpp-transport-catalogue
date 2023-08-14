#pragma once
#include "geo.h"
#include <string>
#include <vector>
#include <set>
#include <unordered_map>

namespace domain {

    struct Stop {
        std::string name;
        geo::Coordinates coordinates;
        std::set<std::string> buses_by_stop;
    };

    struct Bus {
        std::string number;
        std::vector<const Stop*> stops;
        bool is_circle;
    };

    struct RouteInforamtion {
        size_t stops_count;
        size_t unique_stops_count;
        double route_length;
        double curvature;
    };

    struct Hasher {
        size_t operator()(const std::pair<const Stop*, const Stop*>& points) const {
            size_t hash_first = std::hash<const void*>{}(points.first);
            size_t hash_second = std::hash<const void*>{}(points.second);
            return hash_first + hash_second * 37;
        }
    };

}
