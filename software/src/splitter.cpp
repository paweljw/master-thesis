#include <fstream>
#include <iostream>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <sstream>

using namespace std;

int main() 
{
	// This block actually shows how to compress a file with boost

    ifstream file("test.txt", ios_base::in);
	ofstream outf("test.gz", ios_base::out, ios_base::binary);

	boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
	// Can also use gzip_decompressor to decompress the previous stream
	in.push(boost::iostreams::gzip_compressor());
    in.push(file);
    boost::iostreams::copy(in, outf);
}