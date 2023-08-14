#include "request_handler.h"

std::optional<domain::RouteInforamtion> RequestHandler::GetBusStat(const std::string_view bus_number) const {
    domain::RouteInforamtion bus_stat{};
    const domain::Bus* bus = catalogue_.FindBusRoute(bus_number);

    if (!bus) {
        throw std::invalid_argument("bus not found");
    }
    if (bus->is_circle) bus_stat.stops_count = bus->stops.size();
    else bus_stat.stops_count = bus->stops.size() * 2 - 1;

    int route_length = 0;
    double geographic_length = 0.0;

    for (size_t i = 0; i < bus->stops.size() - 1; ++i) {
        auto from = bus->stops[i];
        auto to = bus->stops[i + 1];
        if (bus->is_circle) {
            route_length += catalogue_.GetStopDistance(from, to);
            geographic_length += geo::ComputeDistance(from->coordinates,
                to->coordinates);
        }
        else {
            route_length += catalogue_.GetStopDistance(from, to) + catalogue_.GetStopDistance(to, from);
            geographic_length += geo::ComputeDistance(from->coordinates,
                to->coordinates) * 2;
        }
    }

    bus_stat.unique_stops_count = catalogue_.UniqueStopsCount(bus_number);
    bus_stat.route_length = route_length;
    bus_stat.curvature = route_length / geographic_length;

    return bus_stat;
}

const std::set<std::string> RequestHandler::GetBusesByStop(std::string_view stop_name) const {
    return catalogue_.FindBusStop(stop_name)->buses_by_stop;
}

bool RequestHandler::BusNumber(const std::string_view bus_number) const {
    return catalogue_.FindBusRoute(bus_number);
}

bool RequestHandler::StopName(const std::string_view stop_name) const {
    return catalogue_.FindBusStop(stop_name);
}

svg::Document RequestHandler::RenderMap() const {
    return renderer_.SVG(catalogue_.GetSortedBuses());
}
