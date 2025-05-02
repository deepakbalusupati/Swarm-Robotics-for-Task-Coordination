from flask import Flask, render_template, jsonify
import subprocess
import json
import threading
import time

app = Flask(__name__)

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
        time.sleep(2)

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/api/swarm_data')
def get_swarm_data():
    return jsonify(swarm_data)

if __name__ == '__main__':
    # Start the data update thread
    threading.Thread(target=update_swarm_data, daemon=True).start()
    app.run(host='0.0.0.0', port=5000)
