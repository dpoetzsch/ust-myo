cmake_minimum_required(VERSION 2.8)
project(myo-server)
include_directories(src)
add_executable(myo-server src/server.cpp)

find_package(Boost 1.55 COMPONENTS system thread REQUIRED)
message(status "** Boost Include: ${Boost_INCLUDE_DIR}")
message(status "** Boost Libraries: ${Boost_LIBRARY_DIRS}")
message(status "** Boost Libraries: ${Boost_LIBRARIES}")

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
target_link_libraries(myo-server ${Boost_LIBRARIES})

#if (WIN32)
#    set(Boost_USE_STATIC_LIBS        OFF)
#    set(Boost_USE_MULTITHREADED      ON)
#    set(Boost_USE_STATIC_RUNTIME     OFF)
#    set(BOOST_ALL_DYN_LINK           ON)   # force dynamic linking for all libraries
#endif()