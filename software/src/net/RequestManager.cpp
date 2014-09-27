#include <string>
#include <fstream>
#include <list>
#include "lib/inih/INIReader.h"
#include "net/RequestManager.hpp"
#include "db/DatabaseAccessor.hpp"
#include "db/SQLiteDatabaseAccessor.hpp"
#include "models/Task.hpp"

using namespace logging::trivial;
src::severity_logger< severity_level > lg;

namespace horizon
{
	namespace net
	{
		INIReader reader("server_config.ini");
		horizon::db::DatabaseAccessor *dao = NULL;

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

		int request_package_by_part(struct mg_connection *conn)
		{
			Parameters params = parse_querystring(conn);
			std::string code = get_param(params, "auth");

			if(auth(code))
			{
				std::string wave = get_param(params, "wave");
				std::string part = get_param(params, "part");

				if (!horizon::is_numeric(wave) || !horizon::is_numeric(part))
				{
					BOOST_LOG_SEV(lg, info) << "PKGB " << conn->uri << conn->query_string << ": " << __FUNCTIONW__ << "+" << __LINE__;
					mg_printf_data(conn, "%s", "Bad param");
					return MG_TRUE;
				}

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

		int request_package_by_id(struct mg_connection *conn)
		{
			Parameters params = parse_querystring(conn);
			std::string code = get_param(params, "auth");

			if (auth(code))
			{
				std::string task_id = get_param(params, "task_id");

				if (!horizon::is_numeric(task_id))
				{
					BOOST_LOG_SEV(lg, info) << "PKGIDB " << conn->uri << "?" << conn->query_string << "'" << task_id << "': " << __FUNCTIONW__ << "+" << __LINE__;
					mg_printf_data(conn, "%s", "Bad param");
					return MG_TRUE;
				}

				if (dao == NULL)
				{
					dao = new horizon::db::SQLiteDatabaseAccessor(reader.Get("paths", "db_file", "server.db"));
				}

				horizon::models::Task task(boost::lexical_cast<int>(task_id));

				try
				{
					dao->FillTask(task);

					BOOST_LOG_SEV(lg, info) << "PKGIDA " << task_id << ": " << __FUNCTIONW__ << "+" << __LINE__;

					std::string wave = boost::lexical_cast<std::string>(task.getWave());
					std::string part = boost::lexical_cast<std::string>(task.getPartNum());

					mg_send_header(conn, "Content-Disposition", ("attachment; filename=" + wave + "_" + part + ".gz").c_str());
					mg_send_file(conn, ("./storage/" + (wave + "_" + part + ".gz")).c_str());
					return MG_MORE;
				} catch(std::exception e) {
					BOOST_LOG_SEV(lg, info) << "PKGIDP " << task_id << ": " << e.what() << " -> " << __FUNCTIONW__ << "+" << __LINE__;
					std::string s = "Bad package " + task_id;
					mg_printf_data(conn, "%s", s.c_str());
					return MG_TRUE;
				}
			} else {
				BOOST_LOG_SEV(lg, info) << "PKGIDU " << conn->uri << " (" << code << ") : " << __FUNCTIONW__ << "+" << __LINE__;
				std::string s = "Unathorized " + code;
				mg_printf_data(conn, "%s", s.c_str());
				return MG_TRUE;
			}
		}

		int request_package_meta(struct mg_connection *conn)
		{
			Parameters params = parse_querystring(conn);
			std::string code = get_param(params, "auth");

			if (auth(code))
			{
				std::string task_id = get_param(params, "task_id");

				if (!horizon::is_numeric(task_id))
				{
					BOOST_LOG_SEV(lg, info) << "PKGMTB " << conn->uri << "?" << conn->query_string << "'" << task_id << "': " << __FUNCTIONW__ << "+" << __LINE__;
					mg_printf_data(conn, "%s", "Bad param");
					return MG_TRUE;
				}

				if (dao == NULL)
				{
					dao = new horizon::db::SQLiteDatabaseAccessor(reader.Get("paths", "db_file", "server.db"));
				}

				horizon::models::Task task(boost::lexical_cast<int>(task_id));

				try
				{
					dao->FillTask(task);

					BOOST_LOG_SEV(lg, info) << "PKGMTA " << task_id << ": " << __FUNCTIONW__ << "+" << __LINE__;

					mg_send_header(conn, "Content-Disposition", ("attachment; filename=" + task.getMetafile()).c_str());
					mg_send_file(conn, ("./storage/" + task.getMetafile()).c_str());
					return MG_MORE;
				} catch (std::exception e) {
					BOOST_LOG_SEV(lg, info) << "PKGMTP " << task_id << ": " << e.what() << " -> " << __FUNCTIONW__ << "+" << __LINE__;
					std::string s = "Bad package " + task_id;
					mg_printf_data(conn, "%s", s.c_str());
					return MG_TRUE;
				}
			}
			else {
				BOOST_LOG_SEV(lg, info) << "PKGMTU " << conn->uri << " (" << code << ") : " << __FUNCTIONW__ << "+" << __LINE__;
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

			// Probably malformed
			if (tokenList.size() % 2) return ret;

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

		int request_task_list(struct mg_connection *conn)
		{
			Parameters params = parse_querystring(conn);
			std::string code = get_param(params, "auth");

			if (auth(code))
			{
				std::string num = get_param(params, "num");

				if (!horizon::is_numeric(num))
				{
					BOOST_LOG_SEV(lg, info) << "GETTASKSBP " << conn->uri << "?" << conn->query_string << "'" << num << "': " << __FUNCTIONW__ << "+" << __LINE__;
					mg_printf_data(conn, "%s", "Bad param");
					return MG_TRUE;
				}

				if (dao == NULL)
				{
					dao = new horizon::db::SQLiteDatabaseAccessor(reader.Get("paths", "db_file", "server.db"));
				}

				std::vector<horizon::models::Task> tasks = dao->TaskList(boost::lexical_cast<int>(num));
				dao->MassMarkTasksSent(tasks);
				std::string ret = dao->TaskListToJSON(tasks);

				BOOST_LOG_SEV(lg, info) << "GETTASKA: " << __FUNCTIONW__ << "+" << __LINE__;

				mg_send_header(conn, "Content-Type", "application/json");
				mg_printf_data(conn, "%s", ret.c_str());

				return MG_TRUE;
			} else {
				BOOST_LOG_SEV(lg, info) << "GETTASKSBU " << conn->uri << " (" << code << ") : " << __FUNCTIONW__ << "+" << __LINE__;
				std::string s = "Unathorized " + code;
				mg_printf_data(conn, "%s", s.c_str());
				return MG_TRUE;
			}
		}

		int put_task(struct mg_connection *conn)
		{
			// Check for required data first
			Parameters p = parse_querystring(conn);

			std::string task_id = get_param(p, "task_id");
			std::string au = get_param(p, "auth");

			// check auth and numeric id
			if (!auth(au) || !is_numeric(task_id))
			{
				BOOST_LOG_SEV(lg, info) << "PUTTSK BP " << au << " " << task_id;
				mg_printf_data(conn, "Bad params");
				return MG_TRUE;
			}

			// Open dao
			if (dao == NULL)
			{
				dao = new horizon::db::SQLiteDatabaseAccessor(reader.Get("paths", "db_file", "server.db"));
			}

			// Fill up a task structure
		
			horizon::models::Task task(boost::lexical_cast<int>(task_id));

			try
			{
				dao->FillTask(task);
			} catch (std::exception e) {
				BOOST_LOG_SEV(lg, info) << "PUTTSK BT";
				mg_printf_data(conn, "Bad task id");
				return MG_TRUE;
			}

			if (task.getState() != horizon::models::TASK_SENT)
			{
				BOOST_LOG_SEV(lg, info) << "PUTTSK BTS";
				mg_printf_data(conn, "Bad task state");
				return MG_TRUE;
			}

			// We have a task that we're actually expecting and is in the db, yay

			const char *data;
			int data_len, ofs = 0;
			char var_name[100], file_name[100];

			ofs = mg_parse_multipart(conn->content, conn->content_len, var_name, sizeof(var_name), file_name, sizeof(file_name), &data, &data_len);
			
			if(ofs > 0) 
			{
				mg_printf_data(conn, "var: %s, file_name: %s, size: %d bytes<br>", var_name, file_name, data_len);

				// We'll make our own file name
				std::ofstream of(reader.Get("paths", "storage", "./storage/")
					+ boost::lexical_cast<std::string>(task.getWave())
					+ "_"
					+ boost::lexical_cast<std::string>(task.getPartNum()) 
					+ "_stage.gz", std::ios_base::binary);

				if (!of.is_open())
				{
					BOOST_LOG_SEV(lg, info) << "PUTTSK US";
					mg_printf_data(conn, "Universe failure");
					return MG_TRUE;
				}
				
				for (int i = 0; i < data_len; i++)
					of << data[i];

				of.close();
				
				BOOST_LOG_SEV(lg, info) << "PUTTSK save(" << task_id << ")";

				task.markReceived();
				dao->UpdateTask(task);

				horizon::models::Wave wave(task.getWave());
				wave.decrementTasks();
				dao->UpdateWave(wave);
			}

			return MG_TRUE;
		}

		int put_meta(struct mg_connection *conn)
		{
			// Check for required data first
			Parameters p = parse_querystring(conn);

			std::string task_id = get_param(p, "task_id");
			std::string au = get_param(p, "auth");

			// check auth and numeric id
			if (!auth(au) || !is_numeric(task_id))
			{
				BOOST_LOG_SEV(lg, info) << "PUTMTA BP " << au << " " << task_id;
				mg_printf_data(conn, "Bad params");
				return MG_TRUE;
			}

			// Open dao
			if (dao == NULL)
			{
				dao = new horizon::db::SQLiteDatabaseAccessor(reader.Get("paths", "db_file", "server.db"));
			}

			// Fill up a task structure

			horizon::models::Task task(boost::lexical_cast<int>(task_id));

			try
			{
				dao->FillTask(task);
			}
			catch (std::exception e) {
				BOOST_LOG_SEV(lg, info) << "PUTMTA BT";
				mg_printf_data(conn, "Bad task id");
				return MG_TRUE;
			}
			
			const char *data;
			int data_len, ofs = 0;
			char var_name[100], file_name[100];

			ofs = mg_parse_multipart(conn->content, conn->content_len, var_name, sizeof(var_name), file_name, sizeof(file_name), &data, &data_len);

			if (ofs > 0)
			{
				mg_printf_data(conn, "var: %s, file_name: %s, size: %d bytes<br>", var_name, file_name, data_len);

				std::ofstream of(reader.Get("paths", "storage", "./storage/") + std::string(file_name));

				if (!of.is_open())
				{
					BOOST_LOG_SEV(lg, info) << "PUTMTA US";
					mg_printf_data(conn, "Universe failure");
					return MG_TRUE;
				}

				for (int i = 0; i < data_len; i++)
					of << data[i];

				of.close();

				BOOST_LOG_SEV(lg, info) << "PUTMTA save(" << task_id << ", " << file_name << ")";

				task.setMetafile(file_name);
				dao->UpdateTask(task);
			}

			return MG_TRUE;
		}
	}

}