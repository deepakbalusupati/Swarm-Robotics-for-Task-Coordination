cmake_minimum_required(VERSION 3.10)
project(SwarmRobotics)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Detect platform-specific settings
if(WIN32)
    # For Windows
    set(Boost_USE_STATIC_LIBS ON)
    add_definitions(-D_WIN32_WINNT=0x0601)
endif()

# Find Boost with better error handling
if(NOT DEFINED Boost_ROOT AND DEFINED ENV{BOOST_ROOT})
    set(Boost_ROOT $ENV{BOOST_ROOT})
endif()

find_package(Boost 1.71.0 REQUIRED COMPONENTS system)
if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost not found. Please install Boost 1.71.0 or later.")
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${Boost_INCLUDE_DIRS}
)

add_executable(swarm_simulator 
    src/core/swarm_robotics.cpp
    src/simulation/simulator.cpp
)

target_link_libraries(swarm_simulator
    ${Boost_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
)

# Find threading library in a cross-platform way
find_package(Threads REQUIRED)
if(THREADS_HAVE_PTHREAD_ARG)
    target_compile_options(swarm_simulator PUBLIC "-pthread")
endif()
if(CMAKE_THREAD_LIBS_INIT)
    target_link_libraries(swarm_simulator "${CMAKE_THREAD_LIBS_INIT}")
endif()
