#pragma once

#ifndef __REQUEST_MANAGER_INCLUDED
#define __REQUEST_MANAGER_INCLUDED

#include <string>
#include <vector>
#include <utility>
#include <boost/algorithm/string.hpp>

#include "lib/mongoose/mongoose.h"

#include "utilhead.hpp"

namespace horizon
{
	namespace net
	{
		typedef std::vector< std::pair<std::string, std::string> > Parameters;

		void init_reader(std::string);

		/// Fulfills /hello request
		void request_hello(struct mg_connection *conn);

		/// Fulfills /*.mcx.gz?auth=code requests
		int request_package(struct mg_connection *conn);

		/// Logs static requests
		void request_static(struct mg_connection *conn);

		Parameters parse_querystring(struct mg_connection *conn);
		std::string get_param(Parameters, std::string);

		bool auth(std::string in);

		static int router(struct mg_connection *conn, enum mg_event ev) 
		{
			std::string uri;

			switch (ev)
			{
			case MG_AUTH:
				return MG_TRUE;   // Authorize all requests
			case MG_REQUEST:
				// First handle special cases
				if(boost::find_first(conn->uri, "/hello")) {
					request_hello(conn);
					return MG_TRUE;
				} else if(boost::find_first(conn->uri, "/package")) {
					return request_package(conn);
				}

				request_static(conn);
				return MG_FALSE;
				break;
			default:
				return MG_FALSE;  // Rest of the events are not processed
			}
		}
	}
}

#endif