#include <string>
#include "lib/mongoose/mongoose.h"
#include "net/RequestManager.hpp"
#include "lib/inih/INIReader.h"

/// TODO:
/// - fulfill specified requests

int main()
{	

	INIReader config("server_config.ini");

	struct mg_server *server = mg_create_server(NULL, horizon::net::router);
	
	mg_set_option(server, "document_root", config.Get("paths", "document_root", "./html").c_str());
	mg_set_option(server, "listening_port", config.Get("server", "port", "8080").c_str());

	for (;;) {
		mg_poll_server(server, 1000);  // Infinite loop, Ctrl-C to stop
	}
	mg_destroy_server(&server);

	return 0;
}