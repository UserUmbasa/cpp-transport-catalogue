#include "json_reader.h"

const json::Node& jReader::BaseRequests() const
{
    auto help = &input_.GetRoot().AsMap();
    auto tmp = help->find("base_requests");
    if (tmp == help->end()) {
        return null_;
    }
    return (tmp->second);

}
const json::Node& jReader::StatRequests() const
{
    auto help = &input_.GetRoot().AsMap();
    auto tmp = help->find("stat_requests");
    if (tmp == help->end()) {
        return null_;
    }
    return (tmp->second);
}
const json::Node& jReader::RenderSettings() const
{
    auto help = &input_.GetRoot().AsMap();
    auto tmp = help->find("render_settings");
    if (tmp == help->end()) {
        return null_;
    }
    return (tmp->second);
}
const json::Node& jReader::RoutingSettings() const
{
    auto help = &input_.GetRoot().AsMap();
    auto tmp = help->find("routing_settings");
    if (tmp == help->end()) {
        return null_;
    }
    return (tmp->second);
}
void jReader::FillCatalogue(Catalogue& ctlg) {
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

std::tuple<std::string_view, std::vector<const domain::Stop*>, bool> jReader::FillRoute(const json::Dict& request_map, Catalogue& ctlg) const {
    std::string_view bus_number = request_map.at("name").AsString();
    std::vector<const domain::Stop*> stops;
    for (auto& stop : request_map.at("stops").AsArray()) {
        stops.push_back(ctlg.FindBusStop(stop.AsString()));
    }
    bool circular_route = request_map.at("is_roundtrip").AsBool();
    return std::make_tuple(bus_number, stops, circular_route);
}
renderer::MapRenderer jReader::FillRenderSettings(const json::Node& request) const {
    json::Dict request_ = request.AsMap();
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
    if (request_.at("underlayer_color").IsString())
    {
        render_settings.underlayer_color = request_.at("underlayer_color").AsString();
    }
    // массив
    else if (request_.at("underlayer_color").IsArray())
    {
        const json::Array& underlayer_color = request_.at("underlayer_color").AsArray();
        render_settings.underlayer_color = ParseRgb(underlayer_color);
    }
    else throw std::logic_error("error color");
    render_settings.underlayer_width = request_.at("underlayer_width").AsDouble();
    const json::Array& color_palette = request_.at("color_palette").AsArray();
    for (const auto& color_element : color_palette)
    {
        //строка
        if (color_element.IsString())
        {
            render_settings.color_palette.push_back(color_element.AsString());
        }
        //массив
        else if (color_element.IsArray())
        {
            const json::Array& color_type = color_element.AsArray();
            render_settings.color_palette.push_back(ParseRgb(color_type));
        }
        else throw std::logic_error("error");
    }

    return render_settings;
}

svg::Color jReader::ParseRgb(const json::Array& color) const
{
    svg::Color tmp; //std::variant<std::monostate, std::string,svg::Rgb, svg::Rgba>;
    if (color.size() == 3)
    {
        tmp = svg::Rgb(color[0].AsInt(), color[1].AsInt(), color[2].AsInt());
        return tmp;
    }
    else if (color.size() == 4)
    {
        tmp = svg::Rgba(color[0].AsInt(), color[1].AsInt(), color[2].AsInt(), color[3].AsDouble());
        return tmp;
    }
    else throw std::logic_error("error colortype");
}

void jReader::ProcessRequests(const json::Node& stat_requests, const Catalogue& ctlg) const
{
    json::Array result;
    for (auto& request : stat_requests.AsArray()) {
        const auto& request_map = request.AsMap();
        const auto& type = request_map.at("type").AsString();
        if (type == "Stop") result.push_back(PrintStop(request_map, ctlg).AsMap());
        if (type == "Bus") result.push_back(PrintRoute(request_map, ctlg).AsMap());
        if (type == "Map") result.push_back(PrintMap(request_map, ctlg).AsMap());
        if (type == "Route") result.push_back(PrintRouting(request_map, ctlg).AsMap());
    }
    json::Print(json::Document{ result }, std::cout);
}

const json::Node jReader::PrintRoute(const json::Dict& request, const Catalogue& ctlg) const
{
    json::Node result;
    const std::string& route_number = request.at("name").AsString();
    const int id = request.at("id").AsInt();

    if (!ctlg.FindBusRoute(route_number)) {
        result = json::Builder{}
            .StartDict()
            .Key("request_id").Value(id)
            .Key("error_message").Value("not found")
            .EndDict()
            .Build();
    }
    else {
        const auto& route_info = ctlg.GetBusStat(route_number);
        result = json::Builder{}
            .StartDict()
            .Key("request_id").Value(id)
            .Key("curvature").Value(route_info.curvature)
            .Key("route_length").Value(route_info.route_length)
            .Key("stop_count").Value(static_cast<int>(route_info.stops_count))
            .Key("unique_stop_count").Value(static_cast<int>(route_info.unique_stops_count))
            .EndDict()
            .Build();
    }
    return result;
}

const json::Node jReader::PrintStop(const json::Dict& request, const Catalogue& ctlg) const
{
    json::Node result;
    const std::string& stop_name = request.at("name").AsString();
    const int id = request.at("id").AsInt();

    if (!ctlg.FindBusStop(stop_name)) {
        result = json::Builder{}
            .StartDict()
            .Key("request_id").Value(id)
            .Key("error_message").Value("not found")
            .EndDict()
            .Build();
    }
    else {
        json::Array buses;
        for (const auto& bus : ctlg.FindBusStop(stop_name)->buses_by_stop) { buses.push_back(bus);}
        result = json::Builder{}
            .StartDict()
            .Key("request_id").Value(id)
            .Key("buses").Value(buses)
            .EndDict()
            .Build();
    }
    return result;
}

const json::Node jReader::PrintMap(const json::Dict& request_map, const Catalogue& ctlg) const
{
    json::Node result;
    const int id = request_map.at("id").AsInt();
    std::ostringstream strm;

    renderer::RenderSettings render_settings;
    renderer::MapRenderer renderer_(render_settings);

    svg::Document map = renderer_.SVG(ctlg.GetSortedBuses());
    map.Render(strm);

    result = json::Builder{}
        .StartDict()
        .Key("request_id").Value(id)
        .Key("map").Value(strm.str())
        .EndDict()
        .Build();

    return result;
}

const json::Node jReader::PrintRouting(const json::Dict& request_map, const Catalogue& ctlg) const
{
    json::Node result;
    const int id = request_map.at("id").AsInt();
    const std::string_view stop_from = request_map.at("from").AsString();
    const std::string_view stop_to = request_map.at("to").AsString();
    transport_router::Router router_{RoutingSettings(), ctlg };
    
    const auto& routing = router_.FindRoute(stop_from, stop_to);

    if (!routing) {
        result = json::Builder{}
            .StartDict()
            .Key("request_id").Value(id)
            .Key("error_message").Value("not found")
            .EndDict()
            .Build();
    }
    else {
        json::Array items;
        double total_time = 0.0;
        items.reserve(routing.value().edges.size());
        for (auto& edge_id : routing.value().edges) {
            const graph::Edge<double> edge = router_.GetGraph(edge_id);

            if (edge.quality == 0) {
                items.emplace_back(json::Node(json::Builder{}
                .StartDict()
                    .Key("stop_name").Value(edge.name)
                    .Key("time").Value(edge.weight)
                    .Key("type").Value("Wait")
                    .EndDict()
                    .Build()));

                total_time += edge.weight;
            }
            else {
                items.emplace_back(json::Node(json::Builder{}
                .StartDict()
                    .Key("bus").Value(edge.name)
                    .Key("span_count").Value(static_cast<int>(edge.quality))
                    .Key("time").Value(edge.weight)
                    .Key("type").Value("Bus")
                    .EndDict()
                    .Build()));

                total_time += edge.weight;
            }
        }

        result = json::Builder{}
            .StartDict()
            .Key("request_id").Value(id)
            .Key("total_time").Value(total_time)
            .Key("items").Value(items)
            .EndDict()
            .Build();
    }

    return result;
}
