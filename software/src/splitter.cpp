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
#include <sqlite3.h>

// UTILITIES
#include "utilhead.hpp"

using namespace std;

int main(int argc, char** argv) 
{
	if(argc == 1)
	{
		cout << "Usage: splitter matrix.mtx rhs.mtx [part size]" << endl;
		return 0;
	}

	loggerhead("splitter");

    using namespace logging::trivial;
    src::severity_logger< severity_level > lg;

	// 1.1 MTX file handle is opened
	if(argc < 2)
	{
		BOOST_LOG_SEV(lg, fatal) << "MTX file name is required";
		return 1;
	}

	BOOST_LOG_SEV(lg, info) << "Opening " << argv[1];

	ifstream MTX(argv[1], ios_base::in);

	if(!MTX.good() || MTX.bad() || !MTX.is_open())
	{
		BOOST_LOG_SEV(lg, fatal) << "Unable to open RHS file";
		return 1;
	}

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
	
	if(X != Y)
	{
		BOOST_LOG_SEV(lg, fatal) << "Matrix not square, " << X << "!=" << Y;
		return 2;
	}

	BOOST_LOG_SEV(lg, info) << "Reading MTX at " << X << " by " << Y << ", " << entries << " entries";

	int OnePercent = entries / 100;
	int PartSize = 1024;

	/// TODO: Read RHS filename from argv[2]

	if(argc > 3)
	{
		string sPartSize = argv[3];
		istringstream isPartSize(sPartSize);
		isPartSize >> PartSize;
	}

	BOOST_LOG_SEV(lg, info) << "Part size at " << PartSize;

	int PartsNum = X / PartSize + (X%PartSize ? 1 : 0);

	BOOST_LOG_SEV(lg, info) << "Creating " << PartsNum << " parts";

	vector<ofstream> OutStreams(PartsNum);

	string base_filename = "package_" + formattedNow();

	for(int i=0; i<PartsNum;i++)
	{
		stringstream FileName;
		FileName << base_filename << "_" << i << ".mcx";

		OutStreams[i] = ofstream(FileName.str());
	}

	// Now that we have parts and what not figured out, we need to get the RHS

	if(argc < 3)
	{
		BOOST_LOG_SEV(lg, fatal) << "RHS file name is required";
		return 1;
	}

	ifstream RHS(argv[2], ios_base::in);

	if(!RHS.good() || RHS.bad() || !RHS.is_open())
	{
		BOOST_LOG_SEV(lg, fatal) << "Unable to open RHS file";
		return 1;
	}

	int _X, _Y;

	while(getline(RHS, line))
	{
		if(line[0] != '%')
		{
			istringstream iss(line);
			iss >> _X >> _Y;
			break;
		}
	}

	if(X != _X)
		BOOST_LOG_SEV(lg, warning) << "RHS size not equal to matrix dimensions";

	int rhscounter = 0;
	int OneXPercent = X / 100;

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
		stringstream FileName;
		FileName << base_filename << "_" << i << ".mcx";

		string originalFilename = FileName.str();

		ifstream file(originalFilename, ios_base::in);
		BOOST_LOG_SEV(lg, info) << "Compressing " << originalFilename;

		ofstream outf(originalFilename + ".gz", ios_base::out|ios_base::binary);
		boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
		in.push(boost::iostreams::gzip_compressor());
		in.push(file);
		boost::iostreams::copy(in, outf);

		file.close();
		outf.flush();
		outf.close();

		boost::filesystem::remove(originalFilename);

		/// TODO: Register packages here
	}

	// sqlite3 *database;
	// sqlite3_open("test.db", &database);

}