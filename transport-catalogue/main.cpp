#include "json_reader.h"
#include "request_handler.h"

int main()
{

    info_catalogue::TransportCatalogue catalogue;
    jReader doc(std::cin);
    doc.FillCatalogue(catalogue);

    const auto& stat_requests = doc.StatRequests();
    const auto& renderer = doc.FillRenderSettings(doc.RenderSettings());

    const transport_router::Router router = { doc.RoutingSettings(), catalogue };

    RequestHandler rh(catalogue, renderer, router);
    rh.ProcessRequests(stat_requests);

}
