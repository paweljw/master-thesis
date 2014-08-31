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

# Need: OpenCL, _maybe_ boost
set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "C:/SharedLibraries/boost_1_56_0")

set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "C:/SharedLibraries/boost_1_56_0/stage/lib")

# set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "/usr/include")
# set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "/usr/lib")

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


find_package(SQLITE3 REQUIRED)
if(SQLITE3_FOUND)
	INCLUDE_DIRECTORIES(${SQLITE3_INCLUDE_DIR})
endif()

include_directories("src/")
include_directories("src/models")

IF(MSVC)
	# Enable catching C++ exception
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
ENDIF()

add_executable(splitter 
    src/utilhead.cpp
    src/splitter.cpp)
# target_link_libraries(splitter ${OPENCL_LIBRARIES} ${Boost_LIBRARIES} ${SQLITE3_LIBRARY})
target_link_libraries(splitter ${Boost_LIBRARIES})

add_executable(dbctl 
    src/utilhead.cpp
    src/SQLiteDatabaseAccessor.cpp 
    src/dbctl.cpp)

target_link_libraries(dbctl ${Boost_LIBRARIES} ${SQLITE3_LIBRARY})