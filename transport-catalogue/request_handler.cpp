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

void RequestHandler::ProcessRequests(const json::Node& stat_requests) const
{
    json::Array result;
    for (auto& request : stat_requests.AsArray()) {
        const auto& request_map = request.AsMap();
        const auto& type = request_map.at("type").AsString();
        if (type == "Stop") result.push_back(PrintStop(request_map).AsMap());
        if (type == "Bus") result.push_back(PrintRoute(request_map).AsMap());
        if (type == "Map") result.push_back(PrintMap(request_map).AsMap());
    }

    json::Print(json::Document{ result }, std::cout);
}

const json::Node RequestHandler::PrintRoute(const json::Dict& request) const
{
    json::Node result;
    const std::string& route_number = request.at("name").AsString();
    const int id = request.at("id").AsInt();

    if (!BusNumber(route_number)) {
        result = json::Builder{}
            .StartDict()
            .Key("request_id").Value(id)
            .Key("error_message").Value("not found")
            .EndDict()
            .Build();
    }
    else {
        const auto& route_info = GetBusStat(route_number);
        result = json::Builder{}
            .StartDict()
            .Key("request_id").Value(id)
            .Key("curvature").Value(route_info->curvature)
            .Key("route_length").Value(route_info->route_length)
            .Key("stop_count").Value(static_cast<int>(route_info->stops_count))
            .Key("unique_stop_count").Value(static_cast<int>(route_info->unique_stops_count))
            .EndDict()
            .Build();
    }
    return result;
}

const json::Node RequestHandler::PrintStop(const json::Dict& request) const
{
    json::Node result;
    const std::string& stop_name = request.at("name").AsString();
    const int id = request.at("id").AsInt();

    if (!StopName(stop_name)) {
        result = json::Builder{}
            .StartDict()
            .Key("request_id").Value(id)
            .Key("error_message").Value("not found")
            .EndDict()
            .Build();
    }
    else {
        json::Array buses;
        for (const auto& bus : GetBusesByStop(stop_name)) {
            buses.push_back(bus);
        }
        result = json::Builder{}
            .StartDict()
            .Key("request_id").Value(id)
            .Key("buses").Value(buses)
            .EndDict()
            .Build();
    }
    return result;
}

const json::Node RequestHandler::PrintMap(const json::Dict& request_map) const
{
    json::Node result;
    const int id = request_map.at("id").AsInt();
    std::ostringstream strm;
    svg::Document map = RenderMap();
    map.Render(strm);

    result = json::Builder{}
        .StartDict()
        .Key("request_id").Value(id)
        .Key("map").Value(strm.str())
        .EndDict()
        .Build();

    return result;
}

svg::Document RequestHandler::RenderMap() const {
    return renderer_.SVG(catalogue_.GetSortedBuses());
}
