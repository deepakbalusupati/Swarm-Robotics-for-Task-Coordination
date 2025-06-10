#include "../include/swarm_robotics.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <random>

namespace SwarmRobotics {

SwarmController::SwarmController(int robot_count) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(0.0, 100.0);

  for (int i = 0; i < robot_count; ++i) {
    Robot robot;
    robot.id = i;
    robot.x = dis(gen);
    robot.y = dis(gen);
    robot.battery_level = 100.0;
    robot.capabilities = {1, 2, 3, 4, 5};
    robots.push_back(robot);
  }
}

SwarmController::~SwarmController() {}

void SwarmController::addTask(const Task &task) {
  tasks.push_back(task);
  allocateTasks();
}

void SwarmController::updateRobotState(int robot_id, double x, double y,
                                       double battery) {
  if (robot_id >= 0 && robot_id < robots.size()) {
    robots[robot_id].x = x;
    robots[robot_id].y = y;
    robots[robot_id].battery_level = battery;
  }
}

double SwarmController::calculateTaskCost(const Robot &robot,
                                          const Task &task) const {
  double distance =
      std::sqrt(std::pow(robot.x - task.x, 2) + std::pow(robot.y - task.y, 2));
  double battery_cost = (100.0 - robot.battery_level) / 10.0;
  return distance + battery_cost;
}

void SwarmController::allocateTasks() { decentralizedAllocation(); }

void SwarmController::decentralizedAllocation() {
  for (auto &robot : robots) {
    robot.assigned_tasks.clear();
  }
  task_allocations.clear();

  for (auto &task : tasks) {
    if (task.completed)
      continue;

    int best_robot = -1;
    double min_cost = std::numeric_limits<double>::max();

    for (const auto &robot : robots) {
      if (true) { // All robots can perform all tasks in this example
        double cost = calculateTaskCost(robot, task);
        if (cost < min_cost) {
          min_cost = cost;
          best_robot = robot.id;
        }
      }
    }

    if (best_robot != -1) {
      robots[best_robot].assigned_tasks.push_back(task.id);
      task_allocations[task.id] = best_robot;
      broadcastAllocation(task.id, best_robot);
    }
  }
}

void SwarmController::broadcastAllocation(int task_id, int robot_id) {
  std::cout << "Task " << task_id << " allocated to Robot " << robot_id
            << std::endl;
}

std::vector<int> SwarmController::getRobotTasks(int robot_id) const {
  if (robot_id >= 0 && robot_id < robots.size()) {
    return robots[robot_id].assigned_tasks;
  }
  return {};
}

std::string SwarmController::getSystemStatus() const {
  std::string status;
  status += "Swarm Status:\n";
  status += "Robots: " + std::to_string(robots.size()) + "\n";
  status += "Tasks: " + std::to_string(tasks.size()) + "\n";
  status +=
      "Completed Tasks: " +
      std::to_string(std::count_if(tasks.begin(), tasks.end(),
                                   [](const Task &t) { return t.completed; })) +
      "\n";
  return status;
}

CommunicationHandler::CommunicationHandler(boost::asio::io_context &io_context,
                                           int port)
    : socket(io_context,
             boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port)) {
  try {
    // Set up the socket for proper cross-platform behavior
    boost::asio::socket_base::reuse_address option(true);
    socket.set_option(option);
  } catch (const std::exception &e) {
    std::cerr << "Socket setup error: " << e.what() << std::endl;
  }
}

void CommunicationHandler::sendMessage(const std::string &message,
                                       const std::string &ip, int port) {
  try {
    boost::asio::ip::udp::endpoint remote_endpoint(
        boost::asio::ip::address::from_string(ip), port);
    socket.send_to(boost::asio::buffer(message), remote_endpoint);
  } catch (const std::exception &e) {
    std::cerr << "Send error: " << e.what() << std::endl;
  }
}

void CommunicationHandler::broadcastMessage(const std::string &message) {
  std::cout << "Broadcasting: " << message << std::endl;
}

std::string CommunicationHandler::receiveMessage() {
  try {
    boost::asio::ip::udp::endpoint sender_endpoint;
    size_t length = socket.receive_from(boost::asio::buffer(data_, max_length),
                                        sender_endpoint);
    return std::string(data_, length);
  } catch (const std::exception &e) {
    std::cerr << "Receive error: " << e.what() << std::endl;
    return "";
  }
}
} // namespace SwarmRobotics
