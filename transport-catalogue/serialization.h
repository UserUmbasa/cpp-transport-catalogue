#pragma once

#include "json.h"
#include "transport_catalogue.pb.h"
#include "svg.pb.h"
#include "map_renderer.pb.h"

#include "transport_catalogue.h"
#include "transport_router.h"
#include "map_renderer.h"
#include <fstream>

namespace serialization {

	std::tuple<info_catalogue::TransportCatalogue, transport_router::Router, graph::DirectedWeightedGraph<double>, std::map<std::string, graph::VertexId>> Deserialize(std::istream& input);

	void Serialize(info_catalogue::TransportCatalogue& db, const renderer::MapRenderer& renderer, const transport_router::Router& router, std::ostream& output);
	void SerializeStops(const info_catalogue::TransportCatalogue& db, proto_transport::TransportCatalogue& proto_db);
	void SerializeStopDistances(const info_catalogue::TransportCatalogue& db, proto_transport::TransportCatalogue& proto_db);
	void SerializeBuses(const info_catalogue::TransportCatalogue& db, proto_transport::TransportCatalogue& proto_db);
	void SerializeRouter(const transport_router::Router& router, proto_transport::TransportCatalogue& proto_db);
	void SerializeRenderSettings(const renderer::MapRenderer& renderer, proto_transport::TransportCatalogue& proto_db);

	proto_transport::RouterSettings SerializeRouterSettings(const transport_router::Router& router, proto_transport::TransportCatalogue& proto_db);
	proto_graph::Graph SerializeGraph(const transport_router::Router& router, proto_transport::TransportCatalogue& proto_db);
	
	proto_map::Point SerializePoint(const svg::Point& point);
	proto_map::Color SerializeColor(const svg::Color& color);
	proto_map::Rgb SerializeRgb(const svg::Rgb& rgb);
	proto_map::Rgba SerializeRgba(const svg::Rgba& rgba);


	


	void DeserializeStops(info_catalogue::TransportCatalogue& db, const proto_transport::TransportCatalogue& proto_db);
	void DeserializeStopDistances(info_catalogue::TransportCatalogue& db, const proto_transport::TransportCatalogue& proto_db);
	void DeserializeBuses(info_catalogue::TransportCatalogue& db, const proto_transport::TransportCatalogue& proto_db);
	renderer::MapRenderer DeserializeRenderSettings(renderer::RenderSettings& render_settings, const proto_transport::TransportCatalogue& proto_db);
	svg::Point DeserializePoint(const proto_map::Point& proto_point);
	svg::Color DeserializeColor(const proto_map::Color& proto_color);
	transport_router::Router DeserializeRouterSettings(const proto_transport::TransportCatalogue& proto_db);

	graph::DirectedWeightedGraph<double> DeserializeGraph(const proto_transport::TransportCatalogue& proto_db);
	std::map<std::string, graph::VertexId> DeserializeStopIds(const proto_transport::TransportCatalogue& proto_db);

} // serialization

