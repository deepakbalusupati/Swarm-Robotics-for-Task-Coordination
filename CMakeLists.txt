cmake_minimum_required(VERSION 3.10)
project(SwarmRobotics)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost 1.71.0 REQUIRED COMPONENTS system)

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
    pthread
)
