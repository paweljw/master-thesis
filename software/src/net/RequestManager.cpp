#include <string>
#include <list>
#include "lib/inih/INIReader.h"
#include "net/RequestManager.hpp"

using namespace logging::trivial;
src::severity_logger< severity_level > lg;

namespace horizon
{
	namespace net
	{
		INIReader reader("server_config.ini");

		void request_hello(struct mg_connection *conn)
		{
			std::string uri = std::string(conn->uri);
			boost::trim_left_if(uri, boost::is_any_of("/"));

			std::string s = "Hello world!";
			mg_printf_data(conn, "%s", s.c_str());

			BOOST_LOG_SEV(lg, info) << "HLW " << uri << ": " << __FUNCTIONW__ << "+" << __LINE__;
		}

		void request_static(struct mg_connection *conn)
		{
			BOOST_LOG_SEV(lg, info) << "STC " << conn->uri << ": " << __FUNCTIONW__ << "+" << __LINE__;
		}

		int request_package(struct mg_connection *conn)
		{
			Parameters params = parse_querystring(conn);
			std::string code = get_param(params, "auth");

			if(auth(code))
			{
				std::string wave = get_param(params, "wave");
				std::string part = get_param(params, "part");
				BOOST_LOG_SEV(lg, info) << "PKGA (wave " << wave << ", part " << part << "): " << __FUNCTIONW__ << "+" << __LINE__;

				mg_send_header(conn, "Content-Disposition", ("attachment; filename=" + wave + "_" + part + ".gz").c_str());
				mg_send_file(conn, ("./storage/" + (wave + "_" + part + ".gz")).c_str());
				return MG_MORE;
			} else {
				BOOST_LOG_SEV(lg, info) << "PKGU " << conn->uri << " (" << code << ") : " << __FUNCTIONW__ << "+" << __LINE__;
				std::string s = "Unathorized " + code;
				mg_printf_data(conn, "%s", s.c_str());
				return MG_TRUE;
			}
		}

		Parameters parse_querystring(struct mg_connection *conn)
		{
			Parameters ret = Parameters(0);

			if(conn->query_string == NULL) return ret;

			std::list<std::string> tokenList;

			boost::split(tokenList, conn->query_string, boost::is_any_of("=&"), boost::token_compress_on);

			for ( std::list<std::string>::iterator it = tokenList.begin();
				it != tokenList.end();
				++it)
			{
				std::string param, value;
				param = *it;
				++it;
				value = *it;
				ret.push_back( std::make_pair(param, value) );
			}

			return ret;
		}

		std::string get_param(Parameters p, std::string param)
		{
			for(Parameters::iterator it = p.begin();
				it != p.end();
				++it)
			{
				if((*it).first == param)
					return (*it).second;
			}

			return "";
		}

		bool auth(std::string in)
		{
			int many = reader.GetInteger("auth", "codes", 0);

			for(int i=0; i<many; i++)
			{
				if (in == reader.Get("auth", "code" + boost::lexical_cast<std::string>(i), ""))
					return true;
			}

			return false;
		}
	}

}