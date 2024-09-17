@echo off
REM =====================================================================
REM Batch Script to Reset, Setup, and Build CMake Build Environment
REM =====================================================================

REM Enable delayed variable expansion
setlocal enabledelayedexpansion

REM ---------------------------------------------------------------------
REM Get the directory where the batch script is located
REM ---------------------------------------------------------------------
set "SCRIPT_DIR=%~dp0"

REM Remove trailing backslash if present
if "%SCRIPT_DIR:~-1%"=="\" set "SCRIPT_DIR=%SCRIPT_DIR:~0,-1%"

REM ---------------------------------------------------------------------
REM Define the build directory name
REM ---------------------------------------------------------------------
set "BUILD_DIR=%SCRIPT_DIR%\build"


REM ---------------------------------------------------------------------
REM Delete the existing build directory if it exists
REM ---------------------------------------------------------------------
if exist "%BUILD_DIR%" (
    echo Deleting existing build directory: %BUILD_DIR%
    rmdir /s /q "%BUILD_DIR%"
    if ERRORLEVEL 1 (
        echo Error: Failed to delete the build directory.
        pause
        exit /b 1
    )
) else (
    echo No existing build directory found. Skipping deletion.
)

REM ---------------------------------------------------------------------
REM Create a new build directory
REM ---------------------------------------------------------------------
echo Creating new build directory: %BUILD_DIR%
mkdir "%BUILD_DIR%"
if ERRORLEVEL 1 (
    echo Error: Failed to create the build directory.
    pause
    exit /b 1
)

REM ---------------------------------------------------------------------
REM Change to the build directory
REM ---------------------------------------------------------------------
cd /d "%BUILD_DIR%"
if ERRORLEVEL 1 (
    echo Error: Failed to navigate to the build directory.
    pause
    exit /b 1
)

REM ---------------------------------------------------------------------
REM Run CMake to configure the project
REM ---------------------------------------------------------------------
echo Running CMake Configuration with Build Type: %BUILD_TYPE%
cmake .. -DCMAKE_BUILD_TYPE=%BUILD_TYPE%
if ERRORLEVEL 1 (
    echo Error: CMake configuration failed.
    pause
    exit /b 1
)

REM ---------------------------------------------------------------------
REM Completion Message
REM ---------------------------------------------------------------------
echo CMake configuration and build completed successfully.
echo Your build environment is set up in the 'build' directory.
pause
exit /b 0
