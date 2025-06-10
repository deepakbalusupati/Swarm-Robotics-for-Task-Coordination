from flask import Flask, render_template, jsonify, send_from_directory
import subprocess
import json
import threading
import time
import os
import sys
import platform
import logging

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)

app = Flask(__name__)

# Determine proper path to communicate with the C++ executable
def get_executable_path():
    """Get the correct path to the executable based on the platform"""
    base_dir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    
    if platform.system() == "Windows":
        # Windows executable path (Debug or Release folder in Visual Studio, or build folder)
        paths = [
            os.path.join(base_dir, "build", "Release", "swarm_simulator.exe"),
            os.path.join(base_dir, "build", "Debug", "swarm_simulator.exe"),
            os.path.join(base_dir, "build", "swarm_simulator.exe")
        ]
    else:
        # Linux/Mac executable path
        paths = [
            os.path.join(base_dir, "build", "swarm_simulator")
        ]
    
    for path in paths:
        if os.path.exists(path):
            logger.info(f"Found executable at: {path}")
            return path
    
    logger.warning("Executable not found. Running in simulation mode only.")
    return None

# Load configuration
def load_config():
    """Load configuration from config file"""
    try:
        config_path = os.path.join(
            os.path.dirname(os.path.dirname(os.path.abspath(__file__))), 
            "config", "config.json"
        )
        
        if os.path.exists(config_path):
            with open(config_path, 'r') as f:
                return json.load(f)
        else:
            logger.warning(f"Config file not found at {config_path}, using defaults")
            return {
                "web_interface": {
                    "port": 5000,
                    "update_interval_ms": 2000
                }
            }
    except Exception as e:
        logger.error(f"Error loading config: {e}")
        return {
            "web_interface": {
                "port": 5000,
                "update_interval_ms": 2000
            }
        }

# Mock data - in a real implementation, this would connect to your swarm controller
swarm_data = {
    "robots": [],
    "tasks": [],
    "allocations": {}
}

def update_swarm_data():
    """Simulate updating swarm data"""
    global swarm_data
    while True:
        try:
            # In a real implementation, you would get this from your C++ application
            # For now, we'll just simulate some data
            swarm_data = {
                "robots": [
                    {"id": i, "x": i*10, "y": i*15, "battery": 100 - i*10, "tasks": [i]} 
                    for i in range(5)
                ],
                "tasks": [
                    {"id": i, "x": i*20, "y": i*25, "completed": False}
                    for i in range(5)
                ],
                "allocations": {i: i for i in range(5)}
            }
            
            # Try to get data from the C++ application in the future
            # This would involve some form of IPC (inter-process communication)
            
            config = load_config()
            time.sleep(config["web_interface"]["update_interval_ms"] / 1000)
        except Exception as e:
            logger.error(f"Error in update thread: {e}")
            time.sleep(2)  # Default sleep on error

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/static/<path:path>')
def serve_static(path):
    return send_from_directory('static', path)

@app.route('/api/swarm_data')
def get_swarm_data():
    return jsonify(swarm_data)

@app.route('/health')
def health_check():
    return jsonify({"status": "ok"})

if __name__ == '__main__':
    # Load configuration
    config = load_config()
    port = config["web_interface"]["port"]
    
    # Start the data update thread
    update_thread = threading.Thread(target=update_swarm_data, daemon=True)
    update_thread.start()
    
    # Print instructions
    logger.info(f"Web interface running at http://localhost:{port}")
    logger.info("Make sure the swarm_simulator is running separately.")
    
    # Run the Flask app
    app.run(host='0.0.0.0', port=port)
