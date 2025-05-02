#include "../../include/swarm_robotics.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <random>  // Added missing include

using namespace SwarmRobotics;

void robotSimulationThread(SwarmController& swarm, int robot_id) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> pos_dis(0.0, 100.0);
    std::uniform_real_distribution<> battery_dis(20.0, 100.0);

    while (true) {
        double x = pos_dis(gen);
        double y = pos_dis(gen);
        double battery = battery_dis(gen);
        
        swarm.updateRobotState(robot_id, x, y, battery);
        std::this_thread::sleep_for(std::chrono::milliseconds(500 + (gen() % 1500)));
    }
}

int main() {
    const int NUM_ROBOTS = 5;
    SwarmController swarm(NUM_ROBOTS);

    std::vector<std::thread> robot_threads;
    for (int i = 0; i < NUM_ROBOTS; ++i) {
        robot_threads.emplace_back(robotSimulationThread, std::ref(swarm), i);
    }

    Task task1{1, "Inspect area A", 1, 10.0, 10.0, false};
    Task task2{2, "Transport item to B", 2, 30.0, 40.0, false};
    Task task3{3, "Clean area C", 3, 70.0, 20.0, false};
    Task task4{4, "Monitor perimeter", 1, 50.0, 50.0, false};

    swarm.addTask(task1);
    swarm.addTask(task2);
    swarm.addTask(task3);
    swarm.addTask(task4);

    while (true) {
        std::cout << swarm.getSystemStatus() << std::endl;
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> task_dis(1, 4);
        int random_task = task_dis(gen);
        
        for (int i = 0; i < NUM_ROBOTS; ++i) {
            auto tasks = swarm.getRobotTasks(i);
            if (!tasks.empty()) {
                std::cout << "Robot " << i << " has tasks: ";
                for (auto t : tasks) std::cout << t << " ";
                std::cout << std::endl;
            }
        }
        
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    for (auto& t : robot_threads) {
        if (t.joinable()) t.join();
    }

    return 0;
}
