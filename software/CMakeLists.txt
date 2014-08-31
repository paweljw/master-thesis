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

SET(OPENCL_VENDOR "NVIDIA")
FIND_PACKAGE(OpenCL REQUIRED)
INCLUDE_DIRECTORIES(${OPENCL_INCLUDE_DIR})

SET(Boost_USE_STATIC_LIBS ON)
FIND_PACKAGE(Boost 1.56.0 COMPONENTS iostreams system thread date_time regex serialization filesystem timer chrono log REQUIRED)

IF (Boost_FOUND)
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
	LINK_DIRECTORIES ( ${Boost_LIBRARY_DIRS})
    ADD_DEFINITIONS( -DHAS_BOOST )
    add_definitions(-DBOOST_THREAD_USE_LIB)
    add_definitions(-DBOOST_DATE_TIME_NO_LIB)
    ADD_DEFINITIONS (-DBOOST_CHRONO_INLINED )
    ADD_DEFINITIONS (-DBOOST_CHRONO_HEADER_ONLY )
ENDIF()

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

include_directories("src/")
include_directories("src/lib")

IF(MSVC)
	# Enable catching C++ exception
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
ENDIF()

add_executable(splitter 
    src/utilhead.cpp
    src/splitter.cpp)
# target_link_libraries(splitter ${OPENCL_LIBRARIES} ${Boost_LIBRARIES})
target_link_libraries(splitter ${Boost_LIBRARIES})

add_executable(dbctl
    src/lib/sqlite/sqlite3.c
    src/utilhead.hpp 
    src/utilhead.cpp
    src/DatabaseAccessor.hpp
    src/SQLiteDatabaseAccessor.hpp
    src/SQLiteDatabaseAccessor.cpp
    src/SQLiteDBScheme.hpp 
    src/models/Solution.hpp
    src/models/Solution.cpp
    src/models/Wave.hpp
    src/models/Wave.cpp
    src/models/Task.hpp
    src/models/Task.cpp
    src/dbctl.cpp)

target_link_libraries(dbctl ${Boost_LIBRARIES})