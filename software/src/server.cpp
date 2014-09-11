#include "lib/mongoose/mongoose.h"
#include <string>
#include <boost/algorithm/string/trim.hpp>
#include "utilhead.hpp"

std::string document_root = ".";

/// TODO:
/// - include inih
/// - read server data from config
/// - fulfill specified requests
/// - redirect package requests to storage directory
/// - include server config in ini files
/// - clean up the damn build folder

using namespace logging::trivial;
src::severity_logger< severity_level > lg;

static int event_handler(struct mg_connection *conn, enum mg_event ev) 
{
  std::string uri;

  switch (ev)
  {
	case MG_AUTH:
		return MG_TRUE;   // Authorize all requests
	case MG_REQUEST:
		// get URI to "normal" string
		uri = std::string(conn->uri);

		

		// Trim leading slash
		boost::trim_left_if(uri, boost::is_any_of("/"));

		// First handle special cases
		if(!strcmp(conn->uri, "/hello")) {
			std::string s = "Hello world! " + uri;
			mg_printf_data(conn, "%s", s.c_str());

			BOOST_LOG_SEV(lg, info) << "REQ " << document_root << "/" << uri << ": " << __FUNCTIONW__ << "+" << __LINE__;

			return MG_TRUE;
		} else {
			mg_send_file(conn, (document_root + std::string(conn->uri)).c_str() );
			BOOST_LOG_SEV(lg, info) << "REQ " << document_root << "/" << uri;
		}
		break;
	default:
		return MG_FALSE;  // Rest of the events are not processed
  }
}

int main()
{
	loggerhead("server");
	
	struct mg_server *server = mg_create_server(NULL, event_handler);
	mg_set_option(server, "document_root", document_root.c_str());
	mg_set_option(server, "listening_port", "8080");

	for (;;) {
	mg_poll_server(server, 1000);  // Infinite loop, Ctrl-C to stop
	}
	mg_destroy_server(&server);

	return 0;
}