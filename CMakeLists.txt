cmake_minimum_required(VERSION 3.14)
project(IliadEngine)

# C++ Standard
set(CMAKE_CXX_STANDARD 20)

# FetchContent module to retrieve dependencies
include(FetchContent)

# Vulkan
find_package(Vulkan REQUIRED)

# Fetch GLFW
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
)
FetchContent_MakeAvailable(glfw)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

# Fetch ImGui
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.91.1
)
FetchContent_MakeAvailable(imgui)

# Fetch GLM (OpenGL Mathematics)
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.1
)
FetchContent_MakeAvailable(glm)

# Fetch tinyobjloader
FetchContent_Declare(
    tinyobjloader
    GIT_REPOSITORY https://github.com/tinyobjloader/tinyobjloader.git
    GIT_TAG        v2.0-rc1
)
FetchContent_MakeAvailable(tinyobjloader)

# Fetch stb (Single-file public domain libraries)
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(stb)

# Define the source and asset directories
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/IliadEngine")
set(ASSETS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Assets")
set(SHADER_DIR "${ASSETS_DIR}/Shaders")
set(PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/IliadProject")

# Collect all source files from IliadEngine directory
file(GLOB_RECURSE LIB_SOURCES
    "${SOURCE_DIR}/*.cpp"
    "${SOURCE_DIR}/*.h"
)

# Collect shader files
file(GLOB_RECURSE SHADER_FILES "${SHADER_DIR}/*.frag" "${SHADER_DIR}/*.vert")

# Define ImGui source files
set(IMGUI_FILES
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
)

# Add ImGui library
add_library(ImGui STATIC ${IMGUI_FILES})
target_include_directories(ImGui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)
target_link_libraries(ImGui PUBLIC glfw Vulkan::Vulkan)

# Add static library IliadEngine with library sources
add_library(IliadEngine STATIC ${LIB_SOURCES})

# Include directories for IliadEngine
target_include_directories(IliadEngine PUBLIC
    ${Vulkan_INCLUDE_DIRS}
    ${glfw_SOURCE_DIR}/include
    ${glm_SOURCE_DIR}             # Include GLM headers
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
    ${tinyobjloader_SOURCE_DIR}   # Include tinyobjloader headers
    ${stb_SOURCE_DIR}             # Include stb headers
    ${SOURCE_DIR}                 # Include IliadEngine directory headers
)

# Link Vulkan, GLFW, ImGui, and tinyobjloader libraries to IliadEngine
target_link_libraries(IliadEngine PUBLIC Vulkan::Vulkan glfw ImGui tinyobjloader)

# Additional CMake configurations for Vulkan (optional if needed)
if(WIN32)
    target_compile_definitions(IliadEngine PRIVATE VK_USE_PLATFORM_WIN32_KHR)
endif()

# Create the executable project IliadProject with sources from IliadProject directory
file(GLOB_RECURSE PROJECT_SOURCES
    "${PROJECT_DIR}/*.cpp"
    "${PROJECT_DIR}/*.h"
)

add_executable(IliadProject ${PROJECT_SOURCES})

# Include directories for IliadProject
target_include_directories(IliadProject PUBLIC
    ${Vulkan_INCLUDE_DIRS}
    ${glfw_SOURCE_DIR}/include
    ${glm_SOURCE_DIR}             # Include GLM headers
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
    ${stb_SOURCE_DIR}             # Include stb headers
    ${PROJECT_DIR}                # Include IliadProject directory headers
    ${SOURCE_DIR}                 # Include IliadEngine headers
)

# Link IliadProject to IliadEngine, Vulkan, GLFW, and ImGui
target_link_libraries(IliadProject PRIVATE IliadEngine Vulkan::Vulkan glfw ImGui)

if(WIN32)
    target_compile_definitions(IliadProject PRIVATE VK_USE_PLATFORM_WIN32_KHR)
endif()

# ------------------------------------------------------------------------
# Visual Studio Specific Configurations
# ------------------------------------------------------------------------
if(MSVC)
    # Create source groups for Visual Studio filters based on folder structure for IliadEngine
    foreach(source_file ${LIB_SOURCES})
        get_filename_component(dir ${source_file} DIRECTORY)
        file(RELATIVE_PATH rel_dir ${SOURCE_DIR} ${dir})
        if(rel_dir STREQUAL "" OR rel_dir STREQUAL ".")
            set(group_name "\\")
        else()
            # Replace '/' with '\' for Visual Studio compatibility
            string(REPLACE "/" "\\" group_name ${rel_dir})
        endif()
        source_group("${group_name}" FILES "${source_file}")
    endforeach()

    # Create source groups for Visual Studio filters based on folder structure for IliadProject
    foreach(source_file ${PROJECT_SOURCES})
        get_filename_component(dir ${source_file} DIRECTORY)
        file(RELATIVE_PATH rel_dir ${PROJECT_DIR} ${dir})
        if(rel_dir STREQUAL "" OR rel_dir STREQUAL ".")
            set(group_name "\\")
        else()
            string(REPLACE "/" "\\" group_name ${rel_dir})
        endif()
        source_group("${group_name}" FILES "${source_file}")
    endforeach()

    # Group ImGui files into their own 'ImGui' filter
    source_group("ImGui" FILES ${IMGUI_FILES})

    # ------------------------------------------------------------------------
    # Custom Build Steps for Shader Compilation and Asset Copying
    # ------------------------------------------------------------------------

    # Add a pre-build step to compile shaders using CompileShaders.bat
    add_custom_command(TARGET IliadProject PRE_BUILD
        COMMAND "${CMAKE_COMMAND}" -E echo "Compiling shaders..."
    )

    add_custom_command(TARGET IliadProject PRE_BUILD
        COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/CompileShaders.bat"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMENT "Running CompileShaders.bat to compile shaders..."
        VERBATIM
        DEPENDS ${SHADER_FILES}
    )

    # Add a pre-build step to copy assets after shader compilation
    add_custom_command(TARGET IliadProject PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${ASSETS_DIR}" "$<TARGET_FILE_DIR:IliadProject>/Assets"
        COMMENT "Copying Assets to the executable directory..."
        VERBATIM
    )
endif()

# Set working directory to the location of the executable (for Visual Studio .sln files)
set_property(TARGET IliadProject PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:IliadProject>")
