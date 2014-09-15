#define BOOST_TEST_MODULE database_control test
#include <boost/test/unit_test.hpp>

// #define _CRTDBG_MAP_ALLOC

#include <stdlib.h>
#include <crtdbg.h>

#include "db/ServerDatabaseAccessor.hpp"
#include "db/ServerSQLiteDatabaseAccessor.hpp"

#include "models/Solution.hpp"
#include "models/Wave.hpp"
#include "models/Task.hpp"

#include <boost/filesystem.hpp>

BOOST_AUTO_TEST_CASE( db_write_test )
{
	horizon::db::ServerDatabaseAccessor* db1 = new horizon::db::ServerSQLiteDatabaseAccessor("unit_test.db");
	BOOST_REQUIRE( boost::filesystem::exists("unit_test.db") );
}

BOOST_AUTO_TEST_CASE( db_recreate )
{
	horizon::db::ServerDatabaseAccessor* db2 = new horizon::db::ServerSQLiteDatabaseAccessor("unit_test2.db");
	BOOST_REQUIRE( db2->RecreateDatabase() );
}

BOOST_AUTO_TEST_CASE( db_solution )
{
	horizon::db::ServerDatabaseAccessor* db3 = new horizon::db::ServerSQLiteDatabaseAccessor("unit_test3.db");
	BOOST_REQUIRE( db3->RecreateDatabase() );

	horizon::models::Solution solution;
	solution.setWave(1);
	solution.markAvailable();
	solution.setName("testSolution");

	db3->RegisterSolution(solution);

	solution.setWave(5);
	db3->UpdateSolution(solution);

	BOOST_REQUIRE_GT( solution.getID(), 0);

	horizon::models::Solution solution2;

	solution2.setID(solution.getID());

	db3->FillSolution(solution2);

	BOOST_REQUIRE_EQUAL( solution2.getID(), solution.getID() );
	BOOST_REQUIRE_EQUAL( solution2.getWave(), 5 );
	BOOST_REQUIRE_EQUAL( solution2.getName(), "testSolution" );
	BOOST_REQUIRE_EQUAL( solution2.getState(), horizon::models::SOLUTION_AVAILABLE );
}	