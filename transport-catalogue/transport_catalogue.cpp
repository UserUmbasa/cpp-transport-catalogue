#include "transport_catalogue.h"

void info_catalogue_class::TransportCatalogue::AddBusRoute(const info_struct::Bus& businfo) {
    buses_.push_back(businfo);
    finder_bus_[buses_.back().number] = &buses_.back();

    for (const auto& inf : buses_.back().stops) {
        bus_to_for_stop_[inf].insert(businfo.number);
    }
}

void info_catalogue_class::TransportCatalogue::AddBusStop(const info_struct::Stop& stopinfo) 
{
    stops_.push_back(stopinfo);
    finder_stop_[stops_.back().name] = &stops_.back();
}

const info_struct::Bus* info_catalogue_class::TransportCatalogue::FindBusRoute(const std::string& route_number) const {
    if (finder_bus_.count(route_number)) {
        return finder_bus_.at(route_number);
    }
    else
        return nullptr;
}

info_struct::Stop* info_catalogue_class::TransportCatalogue::FindBusStop(const std::string& stop_name) const {
    if (finder_stop_.count(stop_name)) {
        return finder_stop_.at(stop_name);
    }
    else
        return nullptr;
}

const info_struct::RouteInfo info_catalogue_class::TransportCatalogue::BusRouteInformation(const std::string& route_number) const {

    info_struct::RouteInfo info{};
    auto find_ = finder_bus_.find(route_number);

    auto bus = FindBusRoute(route_number);
    if (!bus) {
        std::cout << "notFound"; return { 0,0,0 };
    }

    if (bus->circular_route) {
        info.stops_count = bus->stops.size();
    }
    else {
        info.stops_count = bus->stops.size() * 2 - 1;
    }

    double length = 0.0;
    double geo_length = 0.0;


    for (size_t i = 1; i < bus->stops.size(); ++i) {

        geo_length += coordinate::ComputeDistance(finder_stop_.at(find_->second->stops[i - 1])->coordinates, finder_stop_.at(find_->second->stops[i])->coordinates);
        if (bus->circular_route != true) {
            length += GetStopDistance(finder_stop_.at(find_->second->stops[i - 1]), finder_stop_.at(find_->second->stops[i])) +
                GetStopDistance(finder_stop_.at(find_->second->stops[i]), finder_stop_.at(find_->second->stops[i - 1]));
        }
        else {
            length += GetStopDistance(finder_stop_.at(find_->second->stops[i - 1]), finder_stop_.at(find_->second->stops[i]));
        }

    }
    if (bus->circular_route != true) {
        geo_length *= 2;
    }

    info.unique_stops_count = UniqueStopsCount(route_number);
    info.route_length = length;
    info.curvature = length / geo_length;
    return info;
}

size_t info_catalogue_class::TransportCatalogue::UniqueStopsCount(const std::string& route_number) const {
    std::unordered_set<std::string>unique;
    for (const auto& stop : finder_bus_.at(route_number)->stops) {
        unique.insert(stop);
    }
    return unique.size();
}

std::set<std::string>info_catalogue_class::TransportCatalogue::BusToStop(const std::string& stop_name) const {
    if (bus_to_for_stop_.count(stop_name)) {
        return bus_to_for_stop_.at(stop_name);
    }
    return {};
}
// добавляет данные расстояния до ближайшей остановки
void info_catalogue_class::TransportCatalogue::SetStopDistance(const info_struct::Stop* from, const info_struct::Stop* to, int dist) 
{
    auto stops_pair = std::make_pair(from, to);
    distance[stops_pair] = dist;
}

int info_catalogue_class::TransportCatalogue::GetStopDistance(const info_struct::Stop* f, const info_struct::Stop* t) const 
{
    auto tmp = std::make_pair(f, t);
    if(distance.count(tmp))
    {
        auto check = distance.find(tmp);
        return check->second;
    }
    else {
        return 0;
    }
}
