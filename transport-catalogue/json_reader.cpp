#include "json_reader.h"

const json::Node& jReader::BaseRequests() const {
    if (!input_.GetRoot().AsMap().count("base_requests")) {
        return null_;
    }
    return input_.GetRoot().AsMap().at("base_requests");
}

const json::Node& jReader::StatRequests() const {
    if (!input_.GetRoot().AsMap().count("stat_requests")) {
        return null_;
    }
    return input_.GetRoot().AsMap().at("stat_requests");
}

const json::Node& jReader::RenderSettings() const {
    if (!input_.GetRoot().AsMap().count("render_settings")) {
        return null_;
    }
    return input_.GetRoot().AsMap().at("render_settings");
}

void jReader::ProcessRequests(const json::Node& stat_requests, RequestHandler& rh) const {
    json::Array result;
    for (auto& request : stat_requests.AsArray()) {
        const auto& request_map = request.AsMap();
        const auto& type = request_map.at("type").AsString();
        if (type == "Stop") {
            result.push_back(PrintStop(request_map, rh).AsMap());
        }

        if (type == "Bus") {
            result.push_back(PrintRoute(request_map, rh).AsMap());
        }

        if (type == "Map") {
            result.push_back(PrintMap(request_map, rh).AsMap());
        }

    }
    json::Print(json::Document{ result }, std::cout);
}

void jReader::FillCatalogue(info_catalogue::TransportCatalogue& ctlg) {
    const json::Array& arr = BaseRequests().AsArray();
    for (auto& request_stops : arr) {
        const auto& request_stops_map = request_stops.AsMap();
        const auto& type = request_stops_map.at("type").AsString();
        if (type == "Stop") {
            auto [stop_name, coordinates, stop_distances] = FillStop(request_stops_map);
            ctlg.AddBusStop(stop_name, coordinates);
        }
    }
    FillStopDistances(ctlg);

    for (auto& request_bus : arr) {
        const auto& request_bus_map = request_bus.AsMap();
        const auto& type = request_bus_map.at("type").AsString();
        if (type == "Bus") {
            auto [bus_number, stops, circular_route] = FillRoute(request_bus_map, ctlg);
            ctlg.AddBusRoute(bus_number, stops, circular_route);
        }
    }
}

std::tuple<std::string_view, geo::Coordinates, std::map<std::string_view, int>> jReader::FillStop(const json::Dict& request_map) const {
    std::string_view stop_name = request_map.at("name").AsString();
    geo::Coordinates coordinates = { request_map.at("latitude").AsDouble(), request_map.at("longitude").AsDouble() };
    std::map<std::string_view, int> stop_distances;
    auto& distances = request_map.at("road_distances").AsMap();
    for (auto& [stop_name, dist] : distances) {
        stop_distances.emplace(stop_name, dist.AsInt());
    }
    return std::make_tuple(stop_name, coordinates, stop_distances);
}

void jReader::FillStopDistances(info_catalogue::TransportCatalogue& ctlg) const {
    const json::Array& arr = BaseRequests().AsArray();
    for (auto& request_stops : arr) {
        const auto& request_stops_map = request_stops.AsMap();
        const auto& type = request_stops_map.at("type").AsString();
        if (type == "Stop") {
            auto [stop_name, coordinates, stop_distances] = FillStop(request_stops_map);
            for (auto& [to_name, dist] : stop_distances) {
                auto from = ctlg.FindBusStop(stop_name);
                auto to = ctlg.FindBusStop(to_name);
                ctlg.SetStopDistance(from, to, dist);
            }
        }
    }
}

std::tuple<std::string_view, std::vector<const domain::Stop*>, bool> jReader::FillRoute(const json::Dict& request_map, info_catalogue::TransportCatalogue& ctlg) const {
    std::string_view bus_number = request_map.at("name").AsString();
    std::vector<const domain::Stop*> stops;
    for (auto& stop : request_map.at("stops").AsArray()) {
        stops.push_back(ctlg.FindBusStop(stop.AsString()));
    }
    bool circular_route = request_map.at("is_roundtrip").AsBool();

    return std::make_tuple(bus_number, stops, circular_route);
}

renderer::MapRenderer jReader::FillRenderSettings(const json::Dict& request_) const {
    renderer::RenderSettings render_settings;
    render_settings.width = request_.at("width").AsDouble();
    render_settings.height = request_.at("height").AsDouble();
    render_settings.padding = request_.at("padding").AsDouble();
    render_settings.stop_radius = request_.at("stop_radius").AsDouble();
    render_settings.line_width = request_.at("line_width").AsDouble();
    render_settings.bus_label_font_size = request_.at("bus_label_font_size").AsInt();
    const json::Array& bus_label_offset = request_.at("bus_label_offset").AsArray();
    render_settings.bus_label_offset = { bus_label_offset[0].AsDouble(), bus_label_offset[1].AsDouble() };
    render_settings.stop_label_font_size = request_.at("stop_label_font_size").AsInt();
    const json::Array& stop_label_offset = request_.at("stop_label_offset").AsArray();
    render_settings.stop_label_offset = { stop_label_offset[0].AsDouble(), stop_label_offset[1].AsDouble() };

    if (request_.at("underlayer_color").IsString()) render_settings.underlayer_color = request_.at("underlayer_color").AsString();
    else if (request_.at("underlayer_color").IsArray()) {
        const json::Array& underlayer_color = request_.at("underlayer_color").AsArray();
        if (underlayer_color.size() == 3) {
            render_settings.underlayer_color = svg::Rgb(underlayer_color[0].AsInt(), underlayer_color[1].AsInt(), underlayer_color[2].AsInt());
        }
        else if (underlayer_color.size() == 4) {
            render_settings.underlayer_color = svg::Rgba(underlayer_color[0].AsInt(), underlayer_color[1].AsInt(), underlayer_color[2].AsInt(), underlayer_color[3].AsDouble());
        }
        else throw std::logic_error("error colortype");
    }
    else throw std::logic_error("error color");

    render_settings.underlayer_width = request_.at("underlayer_width").AsDouble();

    const json::Array& color_palette = request_.at("color_palette").AsArray();
    for (const auto& color_element : color_palette) {
        if (color_element.IsString()) render_settings.color_palette.push_back(color_element.AsString());
        else if (color_element.IsArray()) {
            const json::Array& color_type = color_element.AsArray();
            if (color_type.size() == 3) {
                render_settings.color_palette.push_back(svg::Rgb(color_type[0].AsInt(), color_type[1].AsInt(), color_type[2].AsInt()));
            }
            else if (color_type.size() == 4) {
                render_settings.color_palette.push_back(svg::Rgba(color_type[0].AsInt(), color_type[1].AsInt(), color_type[2].AsInt(), color_type[3].AsDouble()));
            }
            else throw std::logic_error("error type");
        }
        else throw std::logic_error("error");
    }

    return render_settings;
}

const json::Node jReader::PrintRoute(const json::Dict& request_map, RequestHandler& rh) const {
    json::Dict result;
    const std::string& route_number = request_map.at("name").AsString();
    result["request_id"] = request_map.at("id").AsInt();
    if (!rh.BusNumber(route_number)) {
        result["error_message"] = json::Node{ static_cast<std::string>("not found") };
    }
    else {
        result["curvature"] = rh.GetBusStat(route_number)->curvature;
        result["route_length"] = rh.GetBusStat(route_number)->route_length;
        result["stop_count"] = static_cast<int>(rh.GetBusStat(route_number)->stops_count);
        result["unique_stop_count"] = static_cast<int>(rh.GetBusStat(route_number)->unique_stops_count);
    }

    return json::Node{ result };
}



const json::Node jReader::PrintStop(const json::Dict& request_, RequestHandler& rh) const {
    json::Dict result;
    const std::string& stop_name = request_.at("name").AsString();
    result["request_id"] = request_.at("id").AsInt();
    if (!rh.StopName(stop_name)) {
        result["error_message"] = json::Node{ static_cast<std::string>("not found") };
    }
    else {
        json::Array buses;
        for (auto& bus : rh.GetBusesByStop(stop_name)) {
            buses.push_back(bus);
        }
        result["buses"] = buses;
    }

    return json::Node{ result };
}

const json::Node jReader::PrintMap(const json::Dict& request_, RequestHandler& rh) const {
    json::Dict result;
    result["request_id"] = request_.at("id").AsInt();
    std::ostringstream strm;
    svg::Document map = rh.RenderMap();
    map.Render(strm);
    result["map"] = strm.str();

    return json::Node{ result };
}
