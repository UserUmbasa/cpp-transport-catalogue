namespace info_catalogue_class
{
	namespace out
	{
		namespace bus
		{
			struct Bus_Query_Result 
			{
				std::string_view name;
				bool not_found;
				size_t stops_on_route;
				size_t unique_stops;
				size_t route_length;
				double curvature;
			};
			Bus_Query_Result query(TransportCatalogue& catalogue, std::string_view str);
			void Show(Bus_Query_Result bus_info, std::ostream& output);
		}
		namespace stop
		{
			struct Stop_Query_Result 
			{
				std::string_view name;
				bool not_found;
				std::vector <std::string_view> buses_name;
			};
			Stop_Query_Result query(TransportCatalogue& catalogue, std::string_view stop_name);
			void Show(Stop_Query_Result bus_info, std::ostream& output);			
		}
		void Transport(std::istream& input, info_catalogue_class::TransportCatalogue& ctlg);
		void Split_Search(info_catalogue_class::TransportCatalogue& ctlg, std::string_view str);
	}
}
