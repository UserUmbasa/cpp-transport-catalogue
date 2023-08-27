#pragma once

#include "json.h"
#include "transport_catalogue.h"
#include "map_renderer.h"
#include"json_builder.h"
#include <sstream>

class RequestHandler {
public:
    RequestHandler(const info_catalogue::TransportCatalogue& catalogue, const renderer::MapRenderer& renderer) : catalogue_(catalogue), renderer_(renderer) {
    }

    std::optional<domain::RouteInforamtion> GetBusStat(const std::string_view) const; 

    const std::set<std::string> GetBusesByStop(std::string_view) const;
    bool BusNumber(const std::string_view) const;
    bool StopName(const std::string_view) const;

    void ProcessRequests(const json::Node& stat_requests) const;

    const json::Node PrintRoute(const json::Dict& request_map) const;
    const json::Node PrintStop(const json::Dict& request_map) const;
    const json::Node PrintMap(const json::Dict& request_map) const;

    svg::Document RenderMap() const;

private:
    const info_catalogue::TransportCatalogue& catalogue_;
    const renderer::MapRenderer& renderer_;
};
