#define BOOST_NO_CXX11_SCOPED_ENUMS
#define BOOST_NO_SCOPED_ENUMS

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/make_shared.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "lib/inih/INIReader.h"

using namespace std;

int main(int argc, char** argv)
{
	if(argc == 1)
	{
		cout << "Usage: splitter matrix.mtx rhs.mtx part_size storage_dir tmp_dir" << endl;
		return 0;
	}

  int PartSize;
	string sPartSize = argv[3];
	istringstream isPartSize(sPartSize);
	isPartSize >> PartSize;

	std::string STORAGE_DIR = string(argv[4]);
	std::string TMP_DIR = string(argv[5]);

	// Check for MTX file name
	if(argc < 2)
	{
		cout << "MTX file name is required" << endl;
		return 1;
	}

	// Open MTX handle
	cout << "Opening " << argv[1] << endl;

	ifstream MTX(argv[1], ios_base::in);

	if(!MTX.good() || MTX.bad() || !MTX.is_open())
	{
		cout << "Couldn't open RHS file" << endl;
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
		cout << "Matrix not square, " << X << "!=" << Y << endl;
		return 2;
	}

	#pragma region OpenRHS
	cout << "Reading MTX at " << X << " by " << Y << ", " << entries << " entries" << endl;

	int OnePercent = entries / 100;

	if (argc < 3)
	{
		cout << "RHS file name is required" << endl;
		return 1;
	}

	ifstream RHS(argv[2], ios_base::in);

	if (!RHS.good() || RHS.bad() || !RHS.is_open())
	{
		cout << "Unable to open RHS file" << endl;
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
		cout << "RHS size not equal to matrix dimensions" << endl;

	int rhscounter = 0;
	int OneXPercent = X / 100;
	#pragma endregion


	cout << "Part size at " << PartSize << endl;

	// Create parts handles
	int PartsNum = X / PartSize + (X%PartSize ? 1 : 0);

	cout << "Creating " << PartsNum << " parts" << endl;

	vector< boost::shared_ptr<ofstream>> OutStreams;

	boost::filesystem::path base_path(argv[1]);
	string base_filename = base_path.stem().string();

	cout << "Output goes to " << STORAGE_DIR << "solution_descr_" << base_filename << endl;
	ofstream output((STORAGE_DIR + "solution_descr_" + base_filename).c_str());

	for(int partnum=0; partnum<PartsNum;partnum++)
	{
		std::string FileName = base_filename + "_" + boost::lexical_cast<std::string>(partnum) + ".mcx";
		OutStreams.push_back( boost::make_shared<ofstream>((TMP_DIR + FileName).c_str()) );
		cout << "Opening stream " << TMP_DIR << FileName << endl;
	}

	// Write out RHS to package files
	cout << "Writing out RHS to packages" << endl;

	cout << "Focus on one hundred pretty dots:" << endl;

	while(getline(RHS, line))
	{
		if(line[0] != '%')
		{
			double v;
			istringstream iss(line);
			iss >> v;
			*(OutStreams[rhscounter / PartSize]) << v << " ";
			OutStreams[rhscounter / PartSize]->flush();
			rhscounter++;
			if(!(rhscounter%OneXPercent)) cout << ".";
		}
	}

	cout << endl;


	for(int i=0;i<PartsNum;i++)
		*OutStreams[i] << endl << "%" << endl;

	int _localx, _localy, _part;
	double lvalue;
	int count = 0;

	// Write out MTX contents to packages
	cout << "Writing out MTX to packages" << endl;

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
			*OutStreams[_part] << _localx << " " << _localy << " " << lvalue << endl;
			count++;
			if(!(count % OnePercent)) cout << ".";
		}
	}

	cout << endl;

	// Close all open file descriptors
	cout << "Closing currently open file descriptors" << endl;


	MTX.close();
	RHS.close();
	for(int i=0; i<PartsNum; i++)
		OutStreams[i]->close();


	for(int i=0; i<PartsNum;i++)
	{
		std::string FileName = base_filename + "_" + boost::lexical_cast<std::string>(i);

		ifstream file(TMP_DIR + FileName + ".mcx", ios_base::in);
		cout << "Compressing " << TMP_DIR << FileName << ".mcx" << endl;

		ofstream outf(STORAGE_DIR + FileName + ".gz", ios_base::out|ios_base::binary);
		cout << "Compressed " << STORAGE_DIR << FileName << ".gz" << endl;

		output << STORAGE_DIR << FileName << ".gz" << endl;

		boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
		in.push(boost::iostreams::gzip_compressor());
		in.push(file);
		boost::iostreams::copy(in, outf);

		file.close();
		outf.flush();
		outf.close();

		boost::filesystem::remove(TMP_DIR + FileName + ".mcx");
	}

	output.close();

	return 0;
}
