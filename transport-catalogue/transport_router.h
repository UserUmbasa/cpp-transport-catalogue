#pragma once

#include "router.h"
#include"json.h"
#include "transport_catalogue.h"

#include <memory>

namespace transport_router {

	class Router {
	public:
		Router() = default;

		Router(const json::Node& settings, const info_catalogue::TransportCatalogue& catalogue)
		{
			bus_wait_time_ = settings.AsMap().at("bus_wait_time").AsInt();
			bus_velocity_ = settings.AsMap().at("bus_velocity").AsDouble();
			Add_Stop(catalogue);
			Add_Bus(catalogue);
		}
		
		const std::optional<graph::Router<double>::RouteInfo> FindRoute(const std::string_view stop_from, const std::string_view stop_to) const;
		//тут не совсем понял...он нужен в request_handler.cpp 139
		const graph::DirectedWeightedGraph<double>& GetGraph() const;

	private:
		void Add_Stop(const info_catalogue::TransportCatalogue& catalogue) ;
		void Add_Bus(const info_catalogue::TransportCatalogue& catalogue);
		int bus_wait_time_ = 0;
		double bus_velocity_ = 0.0;

		graph::DirectedWeightedGraph<double> graph_;
		std::map<std::string, graph::VertexId> stop_ids_;
		std::unique_ptr<graph::Router<double>> router_;
		
	};
}
