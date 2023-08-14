#include "transport_catalogue.h"
using namespace info_catalogue;

void TransportCatalogue::AddBusStop(std::string_view stop_name, const geo::Coordinates coordinates) {
    stops_.push_back({ std::string(stop_name), coordinates, {} });
    finder_stop_[stops_.back().name] = &stops_.back();
}

void TransportCatalogue::AddBusRoute(std::string_view bus_number, const std::vector<const domain::Stop*> stops, bool is_circle) {
    buses_.push_back({ std::string(bus_number), stops, is_circle });
    finder_bus_[buses_.back().number] = &buses_.back();
    for (const auto& route_stop : stops) {
        for (auto& stop_ : stops_) {
            if (stop_.name == route_stop->name) stop_.buses_by_stop.insert(std::string(bus_number));
        }
    }
}

const domain::Bus* TransportCatalogue::FindBusRoute(std::string_view bus_number) const {
    if (finder_bus_.count(bus_number)) {
        return finder_bus_.at(bus_number);
    }
    else
        return nullptr;
}

const domain::Stop* TransportCatalogue::FindBusStop(std::string_view stop_name) const {
    if (finder_stop_.count(stop_name)) {
        return finder_stop_.at(stop_name);
    }
    else
        return nullptr;
}

size_t TransportCatalogue::UniqueStopsCount(std::string_view bus_number) const {
    std::unordered_set<std::string_view> unique_stops;
    for (const auto& stop : finder_bus_.at(bus_number)->stops) {
        unique_stops.insert(stop->name);
    }
    return unique_stops.size();
}

void TransportCatalogue::SetStopDistance(const domain::Stop* from, const domain::Stop* to, const int dist) {

    auto stops_pair = std::make_pair(from, to);
    stop_distances_[stops_pair] = dist;
}

int TransportCatalogue::GetStopDistance(const domain::Stop* from, const domain::Stop* to) const {
    if (stop_distances_.count({ from, to })) {
        return stop_distances_.at({ from, to });
    }
    else if (stop_distances_.count({ to, from })) {
        return stop_distances_.at({ to, from });
    }
    else
        return 0;
}

const std::map<std::string_view, const domain::Bus*> TransportCatalogue::GetSortedBuses() const {
    std::map<std::string_view, const domain::Bus*> result;
    for (const auto& bus : finder_bus_) {
        result.emplace(bus);
    }
    return result;
}
