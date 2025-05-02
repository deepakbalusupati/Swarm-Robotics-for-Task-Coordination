#ifndef SWARM_ROBOTICS_H
#define SWARM_ROBOTICS_H

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <boost/asio.hpp>

namespace SwarmRobotics {

    // Basic data structures
    struct Task {
        int id;
        std::string description;
        int priority;
        double x, y; // Position
        bool completed;
    };

    struct Robot {
        int id;
        double x, y; // Current position
        double battery_level;
        std::vector<int> capabilities;
        std::vector<int> assigned_tasks;
    };

    // Core swarm class
    class SwarmController {
    public:
        SwarmController(int robot_count);
        ~SwarmController();

        void addTask(const Task& task);
        void updateRobotState(int robot_id, double x, double y, double battery);
        void allocateTasks();
        std::vector<int> getRobotTasks(int robot_id) const;
        std::string getSystemStatus() const;

    private:
        std::vector<Robot> robots;
        std::vector<Task> tasks;
        std::unordered_map<int, int> task_allocations; // task_id -> robot_id

        double calculateTaskCost(const Robot& robot, const Task& task) const;
        void decentralizedAllocation();
        void broadcastAllocation(int task_id, int robot_id);
    };

    // Communication interface
    class CommunicationHandler {
    public:
        CommunicationHandler(boost::asio::io_context& io_context, int port);
        void sendMessage(const std::string& message, const std::string& ip, int port);
        void broadcastMessage(const std::string& message);
        std::string receiveMessage();

    private:
        boost::asio::ip::udp::socket socket;
        boost::asio::ip::udp::endpoint remote_endpoint;
        enum { max_length = 1024 };
        char data_[max_length];
    };

} // namespace SwarmRobotics

#endif // SWARM_ROBOTICS_H
