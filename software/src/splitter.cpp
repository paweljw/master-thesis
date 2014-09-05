// DEFAULTS
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
#include "lib/inih/INIReader.h"

// UTILITIES
#include "utilhead.hpp"

#include "models\Solution.hpp"
#include "models\Wave.hpp"
#include "models\Task.hpp"

#include "ServerDatabaseAccessor.hpp"
#include "ServerSQLiteDatabaseAccessor.hpp"

using namespace std;

int main(int argc, char** argv) 
{
	// Check if we have arguments at all
	if(argc == 1)
	{
		cout << "Usage: splitter matrix.mtx rhs.mtx [part size]" << endl;
		return 0;
	}

	// Set up logging facilities
	loggerhead("splitter");
	using namespace logging::trivial;
	src::severity_logger< severity_level > lg;

	// Set up configuration facilities
	INIReader reader("server_config.ini");
	
	if (reader.ParseError() < 0) {
		BOOST_LOG_SEV(lg, fatal) << "Can't load 'server_config.ini'\n";
		return 1;
	}

	horizon::db::ServerDatabaseAccessor* DAO = new horizon::db::ServerSQLiteDatabaseAccessor(reader.Get("paths", "db_file", "storage/server.s3db"));

	// Read configuration defaults

	int PartSize = reader.GetInteger("matrix", "part_size", 1024);

	if (argc > 3)
	{
		string sPartSize = argv[3];
		istringstream isPartSize(sPartSize);
		isPartSize >> PartSize;
	}

	std::string STORAGE_DIR = reader.Get("paths", "storage", "storage/");
	std::string TMP_DIR = reader.Get("paths", "tmp", "tmp/");

	// Check for MTX file name
	if(argc < 2)
	{
		BOOST_LOG_SEV(lg, fatal) << "MTX file name is required";
		return 1;
	}

	// Open MTX handle
	BOOST_LOG_SEV(lg, info) << "Opening " << argv[1];

	ifstream MTX(argv[1], ios_base::in);

	if(!MTX.good() || MTX.bad() || !MTX.is_open())
	{
		BOOST_LOG_SEV(lg, fatal) << "Unable to open RHS file";
		return 1;
	}

	// Load matrix dimensions
	int X, Y, entries;
	string line;

	while(getline(MTX, line))
	{
		if(line[0] != '%')
		{
			istringstream iss(line);
			iss >> X >> Y >> entries;
			break;
		}
	}
	
	// Check if universe is still sane
	if(X != Y)
	{
		BOOST_LOG_SEV(lg, fatal) << "Matrix not square, " << X << "!=" << Y;
		return 2;
	}

	#pragma region OpenRHS
	BOOST_LOG_SEV(lg, info) << "Reading MTX at " << X << " by " << Y << ", " << entries << " entries";

	int OnePercent = entries / 100;

	if (argc < 3)
	{
		BOOST_LOG_SEV(lg, fatal) << "RHS file name is required";
		return 1;
	}

	ifstream RHS(argv[2], ios_base::in);

	if (!RHS.good() || RHS.bad() || !RHS.is_open())
	{
		BOOST_LOG_SEV(lg, fatal) << "Unable to open RHS file";
		return 1;
	}

	int _X, _Y;

	while (getline(RHS, line))
	{
		if (line[0] != '%')
		{
			istringstream iss(line);
			iss >> _X >> _Y;
			break;
		}
	}

	if (X != _X)
		BOOST_LOG_SEV(lg, warning) << "RHS size not equal to matrix dimensions";

	int rhscounter = 0;
	int OneXPercent = X / 100;
	#pragma endregion

	horizon::models::Solution solution_;
	/// TODO: Need to sanitize this a bit
	solution_.setName(argv[1]);

	DAO->RegisterSolution(solution_);

	horizon::models::Wave wave_;
	wave_.setSeq(1);
	wave_.setSolutionID(solution_.getID());

	DAO->RegisterWave(wave_);

	solution_.setWave(wave_.getID());
	DAO->UpdateSolution(solution_);

	BOOST_LOG_SEV(lg, info) << "Part size at " << PartSize;

	// Create parts handles
	int PartsNum = X / PartSize + (X%PartSize ? 1 : 0);

	BOOST_LOG_SEV(lg, info) << "Creating " << PartsNum << " parts";

	vector<ofstream> OutStreams(PartsNum);

	string base_filename = boost::lexical_cast<std::string>(wave_.getID());

	for(int partnum=0; partnum<PartsNum;partnum++)
	{
		std::string FileName = base_filename + "_" + boost::lexical_cast<std::string>(partnum) + ".mcx";

		OutStreams[partnum] = ofstream(TMP_DIR + FileName);
	}

	// Write out RHS to package files
	BOOST_LOG_SEV(lg, info) << "Writing out RHS to packages";

	cout << "Focus on one hundred pretty dots:" << endl;

	while(getline(RHS, line))
	{
		if(line[0] != '%')
		{
			double v;
			istringstream iss(line);
			iss >> v;
			OutStreams[rhscounter / PartSize] << v << " ";
			rhscounter++;
			if(!(rhscounter%OneXPercent)) cout << ".";
		}
	}

	cout << endl;

	for(int i=0;i<PartsNum;i++)
		OutStreams[i] << endl << "%" << endl;

	int _localx, _localy, _part;
	double lvalue;
	int count = 0;

	// Write out MTX contents to packages
	BOOST_LOG_SEV(lg, info) << "Writing out MTX to packages";

	cout << "Focus on one hundred even prettier dots:" << endl;

	while(getline(MTX, line))
	{
		if(line[0] != '%')
		{
			istringstream iss(line);
			iss >> _localx >> _localy >> lvalue;

			// MTX is 1-indexed
			_localx--;
			_localy--;

			_part = _localy / PartSize;
			OutStreams[_part] << _localx << " " << _localy << " " << lvalue << endl;
			count++;
			if(!(count % OnePercent)) cout << ".";
		}
	}

	cout << endl;

	// Close all open file descriptors
	BOOST_LOG_SEV(lg, info) << "Closing currently open file descriptors";

	MTX.close();
	RHS.close();
	for(int i=0; i<PartsNum; i++) 
		OutStreams[i].close();

	// GZIP and close files

	/// TODO: Register a solution and a wave here
	/// I might want a simple sqlite backend after all

	for(int i=0; i<PartsNum;i++)
	{
		horizon::models::Task task_;
		task_.setName(solution_.getName() + " wave " + boost::lexical_cast<std::string>(wave_.getSeq()) + " part " + boost::lexical_cast<std::string>(i));
		task_.setNode("");
		task_.setPartNum(i);
		task_.setState(horizon::models::TASK_NOT_READY);
		task_.setType(horizon::models::TASK_PART_SOLUTION);
		task_.setWave(wave_.getID());
		DAO->RegisterTask(task_);

		std::string FileName = base_filename + "_" + boost::lexical_cast<std::string>(i);

		ifstream file(TMP_DIR + FileName + ".mcx", ios_base::in);
		BOOST_LOG_SEV(lg, info) << "Compressing " << FileName;

		ofstream outf(STORAGE_DIR + FileName + ".gz", ios_base::out|ios_base::binary);
		boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
		in.push(boost::iostreams::gzip_compressor());
		in.push(file);
		boost::iostreams::copy(in, outf);

		file.close();
		outf.flush();
		outf.close();

		task_.markReady();
		DAO->UpdateTask(task_);

		wave_.incrementTasks();

		boost::filesystem::remove(TMP_DIR + FileName + ".mcx");
	}

	wave_.markAvailable();
	solution_.markAvailable();

	DAO->UpdateSolution(solution_);
	DAO->UpdateWave(wave_);

	return 0;
}