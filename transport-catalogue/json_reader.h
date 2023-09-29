#pragma once

#include "json.h"
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "request_handler.h"
#include "svg.h"
#include"json_builder.h"
#include <iostream>
#include <sstream>
using Catalogue = info_catalogue::TransportCatalogue;

class jReader
{
public:
    jReader(std::istream& input) : input_(json::Load(input)) {
    }

    const json::Node& RenderSettings() const;
    const json::Node& BaseRequests() const;
    const json::Node& StatRequests() const;
    const json::Node& RoutingSettings() const;

    void FillCatalogue(Catalogue& ctlg);
    renderer::MapRenderer FillRenderSettings(const json::Node& request_map) const;
    svg::Color ParseRgb(const json::Array& color) const;
    //--------------print----------
    void ProcessRequests(const json::Node& stat_requests, const Catalogue& ctlg) const;
    const json::Node PrintRoute(const json::Dict& request_map, const Catalogue& ctlg) const;
    const json::Node PrintStop(const json::Dict& request_map, const Catalogue& ctlg) const;
    const json::Node PrintMap(const json::Dict& request_map, const Catalogue& ctlg) const;
    const json::Node PrintRouting(const json::Dict& request_map, const Catalogue& ctlg) const;
    

private:
    json::Document input_;
    json::Node null_ = nullptr;

    std::tuple<std::string_view, geo::Coordinates, std::map<std::string_view, int>> FillStop(const json::Dict& request_map) const;
    void FillStopDistances(info_catalogue::TransportCatalogue& ctlg) const;
    std::tuple<std::string_view, std::vector<const domain::Stop*>, bool> FillRoute(const json::Dict& request_map, Catalogue& ctlg) const;
};
