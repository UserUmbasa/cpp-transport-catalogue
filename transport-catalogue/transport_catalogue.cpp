#include "transport_catalogue.h"

void infocatalogueclass::TransportCatalogue::AddBusRoute(infostruct::Bus& businfo) {
    buses_.push_back(businfo);
    finderbus_[buses_.back().number] = &buses_.back();

    for (const auto& inf : buses_.back().stops) {
        bustoforstop_[inf].insert(businfo.number);
    }
}

void infocatalogueclass::TransportCatalogue::AddBusStop(infostruct::Stop& stopinfo) 
{
    stops_.push_back(stopinfo);
    finderstop_[stops_.back().name] = &stops_.back();
}

const infostruct::Bus* infocatalogueclass::TransportCatalogue::FindBusRoute(const std::string& route_number) const {
    if (finderbus_.count(route_number)) {
        return finderbus_.at(route_number);
    }
    else
        return nullptr;
}

infostruct::Stop* infocatalogueclass::TransportCatalogue::FindBusStop(const std::string& stop_name) const {
    if (finderstop_.count(stop_name)) {
        return finderstop_.at(stop_name);
    }
    else
        return nullptr;
}

const infostruct::RouteInfo infocatalogueclass::TransportCatalogue::BusRouteInformation(const std::string& route_number) const {

    infostruct::RouteInfo info{};
    auto find_ = finderbus_.find(route_number);

    auto bus = FindBusRoute(route_number);
    if (!bus) {
        std::cout << "notFound"; 
        return { 0,0,0,0 };
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

size_t infocatalogueclass::TransportCatalogue::UniqueStopsCount(const std::string& route_number) const {
    std::unordered_set<std::string>unique;
    for (const auto& stop : finderbus_.at(route_number)->stops) {
        unique.insert(stop);
    }
    return unique.size();
}

std::set<std::string>infocatalogueclass::TransportCatalogue::BusToStop(const std::string& stop_name) const {
    if (bustoforstop_.count(stop_name)) {
        return bustoforstop_.at(stop_name);
    }
    return {};
}

void infocatalogueclass::TransportCatalogue::SetStopDistance(infostruct::Stop* from, infostruct::Stop* to, int dist) {
    from->stop_distances[to->name] = dist;
}

int infocatalogueclass::TransportCatalogue::GetStopDistance(const infostruct::Stop* f, const infostruct::Stop* t) const {
    if (f->stop_distances.count(t->name)) {
        return f->stop_distances.at(t->name);
    }
    else if (t->stop_distances.count(f->name)) {
        return t->stop_distances.at(f->name);
    }
    else {
        return 0;
    }
}
