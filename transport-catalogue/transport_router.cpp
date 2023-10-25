#include "transport_router.h"

namespace transport_router {
    void Router::AddStop(const info_catalogue::TransportCatalogue& catalogue) 
    {
        const std::map<std::string_view, const domain::Stop*>& all_stops = catalogue.GetSortedStops();
        graph::DirectedWeightedGraph<double> stops_graph(all_stops.size() * 2);
        graph::VertexId vertex_id = 0;

        for (const auto& [stop_name, stop_info] : all_stops)
        {
            stop_ids_[stop_info->name] = vertex_id;
            const graph::Edge<double> edge { stop_info->name, 0, vertex_id, ++vertex_id, static_cast<double>(routset_.bus_wait_time) };
            stops_graph.AddEdge(edge);
            ++vertex_id;
        }
        graph_ = std::move(stops_graph);
    }

    void Router::AddBus(const info_catalogue::TransportCatalogue& catalogue)
    {
        const std::map<std::string_view, const domain::Bus*>& all_buses = catalogue.GetSortedBuses();
        graph::DirectedWeightedGraph<double> stops_graph = std::move(graph_);
        for_each(all_buses.begin(), all_buses.end(), [&stops_graph, this, &catalogue](const auto& item)
            {
                const auto& bus_info = item.second;
                const auto& stops = bus_info->stops;
                size_t stops_count = stops.size();
                for (size_t i = 0; i < stops_count; ++i) {
                    for (size_t j = i + 1; j < stops_count; ++j) {
                        const domain::Stop* stop_from = stops[i];
                        const domain::Stop* stop_to = stops[j];
                        int dist_sum = 0;
                        int dist_sum_inverse = 0;
                        for (size_t k = i + 1; k <= j; ++k) {
                            dist_sum += catalogue.GetStopDistance(stops[k - 1], stops[k]);
                            dist_sum_inverse += catalogue.GetStopDistance(stops[k], stops[k - 1]);
                        }
                        stops_graph.AddEdge({ bus_info->number,
                                              j - i,
                                              stop_ids_.at(stop_from->name) + 1,
                                              stop_ids_.at(stop_to->name),
                                              static_cast<double>(dist_sum) / (routset_.bus_velocity * (100.0 / 6.0)) });

                        if (!bus_info->is_circle) {
                            stops_graph.AddEdge({ bus_info->number,
                                                  j - i,
                                                  stop_ids_.at(stop_to->name) + 1,
                                                  stop_ids_.at(stop_from->name),
                                                  static_cast<double>(dist_sum_inverse) / (routset_.bus_velocity * (100.0 / 6.0)) });
                        }
                    }
                }
            });
        graph_ = std::move(stops_graph);
        router_ = std::make_unique<graph::Router<double>>(graph_);
    }


    const std::optional<graph::Router<double>::RouteInfo> Router::FindRoute(const std::string_view stop_from, const std::string_view stop_to) const {
        return router_->BuildRoute(stop_ids_.at(std::string(stop_from)), stop_ids_.at(std::string(stop_to)));
    }

    const graph::Edge<double>& Router::GetGraph(const size_t edge_id) const {
        
        return graph_.GetEdge(edge_id);
    }

    const graph::DirectedWeightedGraph<double>& Router::GetGraph() const
    {
        return graph_;
    }

    void Router::SetGraph(const graph::DirectedWeightedGraph<double> graph, const std::map<std::string, graph::VertexId> stop_ids)
    {
        graph_ = graph;
        stop_ids_ = stop_ids;
        router_ = std::make_unique<graph::Router<double>>(graph_);
    }

    const int Router::GetBusWaitTime() const {
        return routset_.bus_wait_time;
    }

    const double Router::GetBusVelocity() const {
        return routset_.bus_velocity;
    }

    const std::map<std::string, graph::VertexId> Router::GetStopIds() const
    {
        return stop_ids_;
    }

}
