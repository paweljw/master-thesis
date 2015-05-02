cmake_minimum_required(VERSION 2.8)
project(reducer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "/home/pjw/master-thesis/software/swarmhost/bin")

if(NOT CMAKE_BUILD_TYPE)
	SET(CMAKE_BUILD_TYPE "Debug")
endif()

# used only on jack
EXECUTE_PROCESS( COMMAND hostname OUTPUT_VARIABLE CMAKE_HOSTNAME )
IF(NOT "${CMAKE_HOSTNAME}" EQUAL "jack")
	SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})
ENDIF()

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

set(SOURCE_FILES reducer.cpp)
add_executable(reducer ${SOURCE_FILES})

