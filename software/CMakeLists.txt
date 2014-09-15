CMAKE_MINIMUM_REQUIRED (VERSION 2.6)
project (MultiVectorAttack)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR})
 
if(NOT CMAKE_BUILD_TYPE)
	SET(CMAKE_BUILD_TYPE "Debug")
endif()

# used only on jack
EXECUTE_PROCESS( COMMAND hostname OUTPUT_VARIABLE CMAKE_HOSTNAME )
IF(NOT "${CMAKE_HOSTNAME}" EQUAL "jack")
	SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})
ENDIF()

set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "C:/SharedLibraries/boost_1_56_0")
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "C:/SharedLibraries/boost_1_56_0/stage/lib")

IF(NOT $ENV{OPENCL_VENDOR} EQUAL "")
    MESSAGE(STATUS "OpenclVendor " $ENV{OPENCL_VENDOR})
    SET(OPENCL_VENDOR $ENV{OPENCL_VENDOR})
ELSE()
    MESSAGE(STATUS "OpenclVendor " $ENV{OPENCL_VENDOR})
    SET(OPENCL_VENDOR "NVIDIA")
ENDIF()

FIND_PACKAGE(OpenCL REQUIRED)
INCLUDE_DIRECTORIES(${OPENCL_INCLUDE_DIR})

SET(Boost_USE_STATIC_LIBS ON)

IF(NOT $ENV{BOOST_VERSION} EQUAL "")
    FIND_PACKAGE(Boost $ENV{BOOST_VERSION} COMPONENTS iostreams system thread date_time regex serialization filesystem timer chrono log REQUIRED)
ELSE()
    FIND_PACKAGE(Boost 1.56.0 COMPONENTS iostreams system thread date_time regex serialization filesystem timer chrono log REQUIRED)
ENDIF()

IF (Boost_FOUND)
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
	LINK_DIRECTORIES ( ${Boost_LIBRARY_DIRS})
    ADD_DEFINITIONS( -DHAS_BOOST )
    add_definitions(-DBOOST_THREAD_USE_LIB)
    add_definitions(-DBOOST_DATE_TIME_NO_LIB)
    ADD_DEFINITIONS (-DBOOST_CHRONO_INLINED )
    ADD_DEFINITIONS (-DBOOST_CHRONO_HEADER_ONLY )
ENDIF()


include_directories("src/")
include_directories("src/lib")

IF(MSVC)
	# Enable catching C++ exception
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
ENDIF()

add_executable(splitter 
    src/lib/sqlite/sqlite3.c
    src/lib/inih/INIReader.cpp
    src/lib/inih/ini.c
    src/utilhead.hpp 
    src/utilhead.cpp
    src/db/ServerDatabaseAccessor.hpp
    src/db/ServerSQLiteDatabaseAccessor.hpp
    src/db/ServerSQLiteDatabaseAccessor.cpp
    src/db/ServerSQLiteDBScheme.hpp 
    src/models/Solution.hpp
    src/models/Solution.cpp
    src/models/Wave.hpp
    src/models/Wave.cpp
    src/models/Task.hpp
    src/models/Task.cpp
    src/splitter.cpp)

target_link_libraries(splitter ${Boost_LIBRARIES})

add_executable(dbctl
    src/lib/sqlite/sqlite3.c
    src/utilhead.hpp 
    src/utilhead.cpp
    src/db/ServerDatabaseAccessor.hpp
    src/db/ServerSQLiteDatabaseAccessor.hpp
    src/db/ServerSQLiteDatabaseAccessor.cpp
    src/db/ServerSQLiteDBScheme.hpp 
    src/models/Solution.hpp
    src/models/Solution.cpp
    src/models/Wave.hpp
    src/models/Wave.cpp
    src/models/Task.hpp
    src/models/Task.cpp
    src/dbctl.cpp)

target_link_libraries(dbctl ${Boost_LIBRARIES})

add_executable(server
	src/lib/mongoose/mongoose.h
	src/lib/mongoose/mongoose.c
	src/utilhead.hpp
	src/utilhead.cpp
	src/server.cpp
	)

target_link_libraries(server ${Boost_LIBRARIES})

add_executable(db_test
    src/lib/sqlite/sqlite3.c
    src/utilhead.hpp 
    src/utilhead.cpp
    src/db/ServerDatabaseAccessor.hpp
    src/db/ServerSQLiteDatabaseAccessor.hpp
    src/db/ServerSQLiteDatabaseAccessor.cpp
    src/db/ServerSQLiteDBScheme.hpp 
    src/models/Solution.hpp
    src/models/Solution.cpp
    src/models/Wave.hpp
    src/models/Wave.cpp
    src/models/Task.hpp
    src/models/Task.cpp
    src/db_test.cpp)

target_link_libraries(db_test ${Boost_LIBRARIES})

enable_testing()

add_test( db_test db_test --detect_memory_leak=0 )

# target_link_libraries(splitter ${OPENCL_LIBRARIES} ${Boost_LIBRARIES})