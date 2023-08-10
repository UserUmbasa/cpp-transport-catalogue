#include "transport_catalogue.h"

void info_catalogue_class::TransportCatalogue::AddBusRoute(const info_struct::Bus&& businfo) {
    buses_.push_back(businfo);
    finderbus_[buses_.back().number] = &buses_.back();

    for (const auto& inf : buses_.back().stops) {
        bustoforstop_[inf].insert(businfo.number);
    }
}

void info_catalogue_class::TransportCatalogue::AddBusStop(info_struct::Stop& stopinfo) 
{
    stops_.push_back(stopinfo);
    finderstop_[stops_.back().name] = &stops_.back();
}

const info_struct::Bus* info_catalogue_class::TransportCatalogue::FindBusRoute(const std::string& route_number) const {
    if (finderbus_.count(route_number)) {
        return finderbus_.at(route_number);
    }
    else
        return nullptr;
}
info_struct::Stop* info_catalogue_class::TransportCatalogue::FindBusStop(const std::string& stop_name) const {
    if (finderstop_.count(stop_name)) {
        return finderstop_.at(stop_name);
    }
    else
        return nullptr;
}

const info_struct::RouteInfo info_catalogue_class::TransportCatalogue::BusRouteInformation(const std::string& route_number) const {

    info_struct::RouteInfo info{};
    auto find_ = finderbus_.find(route_number);

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

        geo_length += coordinate::ComputeDistance(finderstop_.at(find_->second->stops[i - 1])->coordinates, finderstop_.at(find_->second->stops[i])->coordinates);
        if (bus->circular_route != true) {
            length += GetStopDistance(finderstop_.at(find_->second->stops[i - 1]), finderstop_.at(find_->second->stops[i])) +
                GetStopDistance(finderstop_.at(find_->second->stops[i]), finderstop_.at(find_->second->stops[i - 1]));
        }
        else {
            length += GetStopDistance(finderstop_.at(find_->second->stops[i - 1]), finderstop_.at(find_->second->stops[i]));
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
    for (const auto& stop : finderbus_.at(route_number)->stops) {
        unique.insert(stop);
    }
    return unique.size();
}

std::set<std::string>info_catalogue_class::TransportCatalogue::BusToStop(const std::string& stop_name) const {
    if (bustoforstop_.count(stop_name)) {
        return bustoforstop_.at(stop_name);
    }
    return {};
}
void info_catalogue_class::TransportCatalogue::SetStopDistance(info_struct::Stop* from, info_struct::Stop* to, int dist) 
{
    auto stops_pair = std::make_pair(from, to);
    Distance[stops_pair] = dist;

}

int info_catalogue_class::TransportCatalogue::GetStopDistance(const info_struct::Stop* f, const info_struct::Stop* t) const 
{
    auto chek = std::make_pair(f, t);
    if(Distance.count(chek))
    {
        auto S = Distance.find(chek);
        return S->second;
    }
    else {
        return 0;
    }
}
