#pragma once

#include "router.h"
#include"json.h"
#include "transport_catalogue.h"

#include <memory>

namespace transport_router {

	class Router {
	public:
		Router() = default;

		Router(const json::Node& settings, const info_catalogue::TransportCatalogue& catalogue):
		routset_{ settings.AsMap().at("bus_wait_time").AsInt(),settings.AsMap().at("bus_velocity").AsDouble() }		
		{
			AddStop(catalogue);
			AddBus(catalogue);
		}
		
		const std::optional<graph::Router<double>::RouteInfo> FindRoute(const std::string_view stop_from, const std::string_view stop_to) const;
		const graph::Edge<double> & GetGraph(const size_t edge_id) const;

	private:
		void AddStop(const info_catalogue::TransportCatalogue& catalogue) ;
		void AddBus(const info_catalogue::TransportCatalogue& catalogue);
		domain::RoutSet routset_;
		graph::DirectedWeightedGraph<double> graph_;
		std::map<std::string, graph::VertexId> stop_ids_;
		std::unique_ptr<graph::Router<double>> router_;
		
	};
}
