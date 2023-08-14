#pragma once

#include "geo.h"
#include "domain.h"

#include <iostream>
#include <deque>
#include <string>
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <optional>
#include <unordered_set>
#include <set>
#include <map>


namespace info_catalogue 
{
    class TransportCatalogue {
    public:
        void AddBusRoute(std::string_view bus_number, const std::vector<const domain::Stop*> stops, bool is_circle);
        void AddBusStop(std::string_view stop_name, const geo::Coordinates coordinates);

        const domain::Bus* FindBusRoute(std::string_view bus_number) const;
        const domain::Stop* FindBusStop(std::string_view stop_name) const;


        size_t UniqueStopsCount(std::string_view bus_number) const;
        void SetStopDistance(const domain::Stop* one, const domain::Stop* two, const int dist);
        int GetStopDistance(const domain::Stop* one, const domain::Stop* two) const;
        const std::map<std::string_view, const domain::Bus*> GetSortedBuses() const;

    private:
        std::deque<domain::Bus> buses_;
        std::deque<domain::Stop> stops_;
        std::unordered_map<std::string_view, const domain::Bus*> finder_bus_;
        std::unordered_map<std::string_view, const domain::Stop*> finder_stop_;
        std::unordered_map<std::pair<const domain::Stop*, const domain::Stop*>, int, domain::Hasher> stop_distances_;
    };
}
