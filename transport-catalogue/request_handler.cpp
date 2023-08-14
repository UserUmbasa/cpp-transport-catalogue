#include "request_handler.h"


std::optional<domain::RouteInforamtion> RequestHandler::GetBusStat(const std::string_view bus_number) const 
{
    return catalogue_.GetBusStat(bus_number);
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
