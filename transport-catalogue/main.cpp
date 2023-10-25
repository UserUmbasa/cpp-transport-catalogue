

#include <fstream>
#include <iostream>
#include <string_view>
#include "json_reader.h"
#include "json.h"
#include "serialization.h"

using namespace std::literals;

void PrintUsage(std::ostream& stream = std::cerr) {
    stream << "Usage: transport_catalogue [make_base|process_requests]\n"sv;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        PrintUsage();
        return 1;
    }

    const std::string mode(argv[1]);

    if (mode == "make_base"sv) 
    {
        jReader json_input(std::cin);
        info_catalogue::TransportCatalogue catalogue;
        json_input.FillCatalogue(catalogue);

        const transport_router::Router router = { json_input.RoutingSettings(), catalogue };

        const auto& render_settings = json_input.RenderSettings();
        const renderer::MapRenderer renderer = json_input.FillRenderSettings(render_settings);

        const auto& serialization_settings = json_input.SerializationSettings();

        std::ofstream fout(serialization_settings.AsDict().at("file"s).AsString(), std::ios::binary);
        if (fout.is_open()) {
            serialization::Serialize(catalogue, renderer, router, fout);
            
        }
    }
    else if (mode == "process_requests"sv) {

        jReader json_input(std::cin);
        std::ifstream db_file(json_input.SerializationSettings().AsDict().at("file"s).AsString(), std::ios::binary);
        if (db_file) {
            auto [catalogue, router, graph, stop_ids] = serialization::Deserialize(db_file);
            const auto& stat_requests = json_input.StatRequests();
            router.SetGraph(graph, stop_ids);
            json_input.ProcessRequests(stat_requests, catalogue);
        }

    }
    else {
        PrintUsage();
        return 1;
    }
}

