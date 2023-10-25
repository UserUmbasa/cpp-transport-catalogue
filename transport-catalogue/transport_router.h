#pragma once

#include "router.h"
#include"json.h"
#include "transport_catalogue.h"

#include <memory>

namespace transport_router {

	class Router {
	public:
		Router() = default;
		Router(const int bus_wait_time, const double bus_velocity)
			: routset_{ bus_wait_time ,bus_velocity } {}

		Router(const json::Node& settings, const info_catalogue::TransportCatalogue& catalogue):
		routset_{ settings.AsDict().at("bus_wait_time").AsInt(),settings.AsDict().at("bus_velocity").AsDouble() }
		{
			AddStop(catalogue);
			AddBus(catalogue);
		}
		Router(const Router& settings, graph::DirectedWeightedGraph<double> graph, std::map<std::string, graph::VertexId> stop_ids)
			: routset_{ settings.routset_ }
			, graph_(graph)
			, stop_ids_(stop_ids) {
			router_ = std::make_unique<graph::Router<double>>(graph_);
		}
		
		const std::optional<graph::Router<double>::RouteInfo> FindRoute(const std::string_view stop_from, const std::string_view stop_to) const;
		const graph::Edge<double> & GetGraph(const size_t edge_id) const;
		const graph::DirectedWeightedGraph<double>& GetGraph() const;
		void SetGraph(const graph::DirectedWeightedGraph<double> graph, const std::map<std::string, graph::VertexId> stop_ids);

		const int GetBusWaitTime() const;
		const double GetBusVelocity() const;
		const std::map<std::string, graph::VertexId> GetStopIds() const;
	private:
		void AddStop(const info_catalogue::TransportCatalogue& catalogue) ;
		void AddBus(const info_catalogue::TransportCatalogue& catalogue);
		domain::RoutSet routset_;
		graph::DirectedWeightedGraph<double> graph_;
		std::map<std::string, graph::VertexId> stop_ids_;
		std::unique_ptr<graph::Router<double>> router_;
		
	};
}

