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

		/// Fulfills /packageByPart requests
		int request_package_by_part(struct mg_connection *conn);

		/// Fulfills /packageById requests
		int request_package_by_id(struct mg_connection *conn);

		/// Fulfills /packageMeta requests
		int request_package_meta(struct mg_connection *conn);

		/// Logs static requests
		void request_static(struct mg_connection *conn);

		/**
		 * @fn	void put_task(struct mg_connection *conn);
		 *
		 * @brief	Returns task from client.
		 *
		 * @author	PJW
		 * @date	2014-09-22
		 *
		 * @param [in,out]	conn	If non-null, the connection.
		 */

		int put_task(struct mg_connection *conn);

		int put_meta(struct mg_connection *conn);

		int request_task_list(struct mg_connection *conn);

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
				} else if(boost::find_first(conn->uri, "/packageByPart")) {
					return request_package_by_part(conn);
				} else if (boost::find_first(conn->uri, "/packageById")) {
					return request_package_by_id(conn);
				} else if (boost::find_first(conn->uri, "/packageMeta")) {
					return request_package_meta(conn);
				} else if (boost::find_first(conn->uri, "/returnTask")) {
					return put_task(conn);
				} else if (boost::find_first(conn->uri, "/returnMeta")) {
					return put_meta(conn);
				} else if (boost::find_first(conn->uri, "/getTasks")) {
					return request_task_list(conn);
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