@echo off
REM =====================================================================
REM Shader Compilation Script for Vulkan Engine
REM =====================================================================

REM Get the directory where the batch script is located
set "SCRIPT_DIR=%~dp0"

REM Define the path to the glslc compiler
set "GLSLC=E:\Tools\Vulkan\Bin\glslc.exe"

REM Define the shaders directory relative to the script directory
set "SHADER_DIR=%SCRIPT_DIR%IliadEngine\Shaders"

REM Define the output directory relative to the script directory
set "OUTPUT_DIR=%SCRIPT_DIR%Assets\CompiledShaders"

REM =====================================================================
REM Verify that glslc compiler exists
REM =====================================================================
if not exist "%GLSLC%" (
    echo Error: glslc compiler not found at "%GLSLC%"
    pause
    exit /b 1
)

REM =====================================================================
REM Create the output directory if it doesn't exist
REM =====================================================================
if not exist "%OUTPUT_DIR%" (
    echo Creating output directory at "%OUTPUT_DIR%"
    mkdir "%OUTPUT_DIR%"
    if ERRORLEVEL 1 (
        echo Failed to create output directory.
        pause
        exit /b 1
    )
)

REM =====================================================================
REM Find and list all .vert and .frag shader files
REM =====================================================================
echo.
echo Searching for shader files in "%SHADER_DIR%"...

setlocal enabledelayedexpansion
set "shaderList="

REM Collect all .vert files
for %%f in ("%SHADER_DIR%\*.vert") do (
    if exist "%%f" (
        set "shaderList=!shaderList! "%%f""
    )
)

REM Collect all .frag files
for %%f in ("%SHADER_DIR%\*.frag") do (
    if exist "%%f" (
        set "shaderList=!shaderList! "%%f""
    )
)

REM Check if any shaders found
if "!shaderList!"=="" (
    echo No shader files found in "%SHADER_DIR%".
    pause
    exit /b 0
)

REM List all shader files
echo Found the following shader files to compile:
for %%f in (!shaderList!) do (
    echo - %%~nxf
)

REM =====================================================================
REM Compile shaders
REM =====================================================================
echo.
echo Starting shader compilation...

for %%f in (!shaderList!) do (
    echo Compiling %%~nxf...
    "%GLSLC%" "%%f" -o "%OUTPUT_DIR%\%%~nxf.spv"
    
    REM Check if the compilation was successful
    if ERRORLEVEL 1 (
        echo Failed to compile %%~nxf
        pause
        exit /b 1
    )
)

echo.
echo All shaders compiled successfully.
pause
