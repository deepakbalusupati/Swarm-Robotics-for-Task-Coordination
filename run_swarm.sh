#!/bin/bash

# Check if build directory exists
if [ ! -d "build" ]; then
    echo "Creating build directory..."
    mkdir -p build
    cd build
    cmake ..
    make
    cd ..
else
    # Rebuild if needed
    cd build
    make
    cd ..
fi

# Check if the executable exists
if [ ! -f "build/swarm_simulator" ]; then
    echo "Error: swarm_simulator executable not found. Please check the build process."
    exit 1
fi

# Check if Python is installed
if ! command -v python3 &> /dev/null; then
    echo "Error: Python 3 is not installed. Please install Python 3."
    exit 1
fi

# Check if Flask is installed
if ! python3 -c "import flask" &> /dev/null; then
    echo "Installing Flask..."
    pip3 install flask
fi

# Start the simulator and web interface
echo "Starting Swarm Robotics System..."
echo "Starting the simulator in the background..."
./build/swarm_simulator &
SIMULATOR_PID=$!

echo "Starting the web interface..."
cd web_interface
python3 app.py &
WEB_PID=$!

echo "System is running. Press Ctrl+C to stop."
echo "Web interface available at: http://localhost:5000"

# Handle Ctrl+C to gracefully stop all processes
trap "kill $SIMULATOR_PID $WEB_PID; exit" INT TERM

# Wait for both processes
wait 