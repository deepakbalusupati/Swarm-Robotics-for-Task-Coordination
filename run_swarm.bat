@echo off
SETLOCAL

REM Check if build directory exists
IF NOT EXIST build (
    echo Creating build directory...
    mkdir build
    cd build
    cmake .. -G "Visual Studio 17 2022" -A x64
    cmake --build . --config Release
    cd ..
) ELSE (
    cd build
    cmake --build . --config Release
    cd ..
)

REM Check if the executable exists
IF NOT EXIST build\Release\swarm_simulator.exe (
    IF NOT EXIST build\swarm_simulator.exe (
        echo Error: swarm_simulator executable not found. Please check the build process.
        exit /b 1
    )
)

REM Check if Python is installed
python --version >nul 2>&1
IF %ERRORLEVEL% NEQ 0 (
    echo Error: Python is not installed or not in PATH. Please install Python.
    exit /b 1
)

REM Check if Flask is installed
python -c "import flask" >nul 2>&1
IF %ERRORLEVEL% NEQ 0 (
    echo Installing Flask...
    pip install flask
)

REM Start the simulator and web interface
echo Starting Swarm Robotics System...
echo Starting the simulator...

REM Try different possible locations for the executable
IF EXIST build\Release\swarm_simulator.exe (
    start "" "build\Release\swarm_simulator.exe"
) ELSE (
    start "" "build\swarm_simulator.exe"
)

echo Starting the web interface...
cd web_interface
start "" python app.py

echo System is running.
echo Web interface available at: http://localhost:5000
echo Close the command windows to stop the system.

ENDLOCAL 