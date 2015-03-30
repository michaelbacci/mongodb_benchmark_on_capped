cmake_minimum_required(VERSION 2.8)
set(MONGO_BECH_SRC_ROOT ${PROJECT_SOURCE_DIR} CACHE PATH "MongoDb benchmark on capped collection")
set(CMAKE_MODULE_PATH ${MONGO_BECH_SRC_ROOT}/cmake ${CMAKE_MODULE_PATH})

#This is a c++11 project
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#Finding Libraries

find_package(MongoDB REQUIRED)
include_directories(${MongoDB_INCLUDE_DIR})

#MongoDB require BOOST
set(Boost_COMPONENTS thread date_time regex filesystem system serialization iostreams REQUIRED)
find_package(Boost REQUIRED ${Boost_COMPONENTS} )
include_directories(${Boost_INCLUDE_DIR})

#include SSL only 
option(SSL "Do you need to run benchmark using ssl connection with mongo ?" OFF)

if (SSL)
	find_package(OpenSSL)
	if (OPENSSL_FOUND)
	    add_definitions(-DBOOST_NETWORK_ENABLE_HTTPS)
	    include_directories(${OPENSSL_INCLUDE_DIR})
	endif()
endif()

#include Google benchmark library
enable_testing()
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/third_party/benchmark ${CMAKE_CURRENT_BINARY_DIR}/benchmark)
include_directories(${benchmark_SOURCE_DIR}/include ${benchmark_SOURCE_DIR})
#set(gmock_LIBRARIES gmock gmock_main)


add_subdirectory(src)