cmake_minimum_required(VERSION 2.8)
project(reducer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

if(NOT CMAKE_BUILD_TYPE)
	SET(CMAKE_BUILD_TYPE "Debug")
endif()

# used only on jack
EXECUTE_PROCESS( COMMAND hostname OUTPUT_VARIABLE CMAKE_HOSTNAME )
IF(NOT "${CMAKE_HOSTNAME}" EQUAL "jack")
	SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})
ENDIF()

# set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "/usr/local/boost-1.57.0/include/")
# set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "/usr/local/boost-1.57.0/lib/")

SET(Boost_USE_STATIC_LIBS ON)
FIND_PACKAGE(Boost 1.55.0 COMPONENTS iostreams system filesystem REQUIRED)

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

find_package(ZLIB)

include_directories("src/")
include_directories("src/lib")

set(SOURCE_FILES main.cpp)
add_executable(reducer ${SOURCE_FILES})