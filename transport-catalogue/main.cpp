#include "json_reader.h"
#include "request_handler.h"

int main() 
{
        
    info_catalogue::TransportCatalogue catalogue;
    jReader doc(std::cin);

    doc.FillCatalogue(catalogue);

    const auto& stat_requests = doc.StatRequests();
    const auto& render_settings = doc.RenderSettings();
    const auto& renderer = doc.FillRenderSettings(render_settings);
    const auto& routing_settings = doc.FillRoutingSettings(doc.RoutingSettings());
    const transport_router::Router router = { routing_settings, catalogue };

    RequestHandler rh(catalogue, renderer, router);
    rh.ProcessRequests(stat_requests);

}
