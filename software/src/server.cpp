#include <string>
#include "lib/mongoose/mongoose.h"
#include "net/RequestManager.hpp"

/// TODO:
/// - read server data from config
/// - fulfill specified requests
/// - redirect package requests to storage directory
/// - include server config in ini files
/// - clean up the damn build folder

int main()
{	
	struct mg_server *server = mg_create_server(NULL, horizon::net::router);
	mg_set_option(server, "document_root", "./html");
	mg_set_option(server, "listening_port", "8080");

	for (;;) {
	mg_poll_server(server, 1000);  // Infinite loop, Ctrl-C to stop
	}
	mg_destroy_server(&server);

	return 0;
}