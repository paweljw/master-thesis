#define BOOST_NO_CXX11_SCOPED_ENUMS
#define BOOST_NO_SCOPED_ENUMS

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_io.hpp>

using namespace std;

struct left_neq_right : std::exception {
    const char* what() const noexcept {return "left != right\n";}
};

const double NUM_ERR = 0.0000000001;

bool is_zero(double val)
{
    if (val < 0) val *= -1.0f;

    return val > NUM_ERR;
}

int main(int argc, char ** argv)
{
    if (argc == 1)
        cout << "Usage: reducer part_origin part_offender part_size part_number_origin part_number_offender global_size" << endl;

    string part_origin_filename(argv[1]);
    string part_offender_filename(argv[2]);

    int part_size;
    int part_number_origin;
    int part_number_offender;
    int global_size;

    stringstream(string(argv[3])) >> part_size;
    stringstream(string(argv[4])) >> part_number_origin;
    stringstream(string(argv[5])) >> part_number_offender;
    stringstream(string(argv[6])) >> global_size;

    // load origin

    ifstream origin_mcx(part_origin_filename);

    // load origin RHS
    vector<double> origin_rhs = vector<double>((unsigned int)0);

    for(int i=0;i<part_size;i++)
    {
        double tmp;
        origin_mcx >> tmp;
        origin_rhs.push_back(tmp);
    }

    string skip;

    getline(origin_mcx, skip); // skip to end of RHS line (mostly zeroes)
    getline(origin_mcx, skip); // skip %

    boost::numeric::ublas::compressed_matrix<double> origin_matrix((unsigned int)part_size, (unsigned int)global_size);

    string line;
    int origin_offset = part_size * part_number_origin;

    while(getline(origin_mcx, line))
    {
        boost::trim(line);
        string _row;
        unsigned int row, col;
        double val;

        stringstream(line) >> _row;
        if (_row.c_str()[0] == '%') break;

        stringstream(line) >> row >> col >> val;
        row -= origin_offset;
        origin_matrix(row, col) = val;
    }

    // load offender

    ifstream offender_mcx(part_offender_filename);

    // load offender RHS
    vector<double> offender_rhs = vector<double>((unsigned int)0);

    for(int i=0;i<part_size;i++)
    {
        double tmp;
        offender_mcx >> tmp;
        offender_rhs.push_back(tmp);
    }

    getline(offender_mcx, skip); // skip to end of RHS line (mostly zeroes)
    getline(offender_mcx, skip); // skip %

    // load offender matrix

    boost::numeric::ublas::compressed_matrix<double> offender_matrix((unsigned int)part_size, (unsigned int)global_size);

    int offender_offset = part_size * part_number_offender;

    while(getline(offender_mcx, line))
    {
        boost::trim(line);
        string _row;
        unsigned int row, col;
        double val;

        stringstream(line) >> _row;
        if (_row.c_str()[0] == '%') break;

        stringstream(line) >> row >> col >> val;
        // cout << "offender: " << row << ", " << col << endl;
        row -= offender_offset;
        // cout << "offender: " << row << ", " << col << endl;
        offender_matrix(row, col) = val;
    }

    // tricksy matrickses are loaded

    vector< boost::tuple<int, int> > reduction = vector< boost::tuple<int, int> >((unsigned int)0);

    string l, r;

    while(true)
    {
        cin >> l >> r;

        if(l.c_str()[0] == '%') break;

        int left, right;

        stringstream(l) >> left;
        stringstream(r) >> right;

        reduction.push_back( boost::make_tuple(left, right) );
    }

    // cout << "done loading commands from CIN" << endl;

    for(int it = 0; it < reduction.size(); it++) { // this was supposed to be an iterator... bloody OpenMP
        int left = reduction[it].get<0>();
        int right = reduction[it].get<1>();

        // cout << "Doing processing for " << left << ", " << right << endl;

        int left_fn = 0;
        while(left_fn < global_size)
        {
            if(origin_matrix(left, left_fn) == 0) break;
            left_fn += 1;
        }

        int right_fn = 0;
        while(right_fn < global_size)
        {
            if(offender_matrix(right, right_fn) == 0) break;
            right_fn += 1;
        }

        if(left_fn != right_fn)
        {
            cerr << left_fn << " != " << right_fn << " for rows " << left << ", " << right << endl;
            throw new left_neq_right;
        }

        double lower = origin_matrix(left, left_fn);
        double upper = offender_matrix(right, right_fn);

        double multiplier = upper / lower;
        multiplier *= -1.0f;

        for(int i = left_fn; i < global_size; i++)
        {
            if(i==left_fn)
            {
                offender_matrix(right, i) = 0.0f; // this will zero out anyway
            } else {
                if(origin_matrix(left, i) == 0) continue; // zero times anything is zero

                offender_matrix(right, i) += multiplier * origin_matrix(left, i);
            }
        }

        offender_rhs[right] += origin_rhs[left] * multiplier;
    }

    for(int i=0; i<global_size; i++)
    {
        if (i<part_size) cout << offender_rhs[i] << " ";
        else cout << "0 ";
    }

    cout << endl << "%" << endl;

    for(int i = 0; i < part_size; i++)
    {
        for(int j = 0; j < global_size; j++)
        {
            double val = offender_matrix(i, j);
            if (val > NUM_ERR)
            {
                cout << i + offender_offset << " " << j << " " << val << endl;
            }
        }
    }

    return 0;
}