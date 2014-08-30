#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// BOOST FILTERS
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>

#include <boost/filesystem.hpp>

// SQLITE3 BINDINGS
#include <sqlite3.h>

// UTILITIES
#include "utilhead.hpp"

#include "DatabaseAccessor.hpp"
#include "SQLiteDatabaseAccessor.hpp"

using namespace std;

int main(int argc, char** argv)
{
	if (argc == 1)
	{
		cout << "Usage: dbctl file.db command" << endl;
		return 0;
	}

	loggerhead("dbctl");

	using namespace logging::trivial;
	src::severity_logger< severity_level > lg;

	if(argc < 3)
	{
		BOOST_LOG_SEV(lg, fatal) << "Not enough parameters";
		return 1;
	}

	string database = argv[1];
	string command = argv[2];

	horizon::db::SQLiteDatabaseAccessor dao(database);

	if(command == "create")
	{
		dao.CreateDatabase();
	}

	// check for db file

	// try to open it

	// check if we have a command

	// switch into fulfilling the command

	return 0;
}