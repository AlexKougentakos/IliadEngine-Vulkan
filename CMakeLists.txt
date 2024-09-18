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

# Define the source and asset directories
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Source")
set(ASSETS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Assets")
set(SHADER_DIR "${ASSETS_DIR}/Shaders")

# Collect all project source files from Source directory
file(GLOB_RECURSE PROJECT_SOURCES
    "${SOURCE_DIR}/*.cpp"
    "${SOURCE_DIR}/*.h"
    "${SHADER_DIR}/*.frag"
    "${SHADER_DIR}/*.vert"
)

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

# Add executable with project sources and ImGui files
add_executable(IliadEngine ${PROJECT_SOURCES} ${IMGUI_FILES})

# Include directories for GLFW, ImGui, its backends, glm, and your Source directory
target_include_directories(IliadEngine PRIVATE 
    ${glfw_SOURCE_DIR}/include
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
    ${glm_SOURCE_DIR}  # Include GLM headers
    ${SOURCE_DIR}      # Include Source directory headers
)

# Link Vulkan, GLFW, and GLM libraries
target_link_libraries(IliadEngine Vulkan::Vulkan glfw)

# ------------------------------------------------------------------------
# Visual Studio Specific Configurations
# ------------------------------------------------------------------------
if(MSVC)
    # Create source groups for Visual Studio filters based on folder structure
    foreach(source_file ${PROJECT_SOURCES})
        get_filename_component(dir ${source_file} DIRECTORY)
        file(RELATIVE_PATH rel_dir ${SOURCE_DIR} ${dir})
        # Replace '/' with '\' for Visual Studio compatibility
        string(REPLACE "/" "\\" group_name ${rel_dir})
        source_group("${group_name}" FILES "${source_file}")
    endforeach()

    # Group ImGui files into their own 'ImGui' filter
    source_group("ImGui" FILES ${IMGUI_FILES})

    # ------------------------------------------------------------------------
    # Custom Build Steps for Shader Compilation and Asset Copying
    # ------------------------------------------------------------------------

    # Find all shader files (.frag and .vert)
    file(GLOB_RECURSE SHADER_FILES "${SHADER_DIR}/*.frag" "${SHADER_DIR}/*.vert")

    # Add a pre-build step to compile shaders using CompileShaders.bat
    add_custom_command(TARGET IliadEngine PRE_BUILD
        COMMAND "${CMAKE_COMMAND}" -E echo "Compiling shaders..."
    )
    
    add_custom_command(TARGET IliadEngine PRE_BUILD
        COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/CompileShaders.bat"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMENT "Running CompileShaders.bat to compile shaders..."
        VERBATIM
        DEPENDS ${SHADER_FILES}
    )

    # Add a pre-build step to copy assets after shader compilation
    add_custom_command(TARGET IliadEngine PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${ASSETS_DIR}" "$<TARGET_FILE_DIR:IliadEngine>/Assets"
        COMMENT "Copying Assets to the executable directory..."
        VERBATIM
    )
endif()

# ------------------------------------------------------------------------
# Post-Build Step to Copy Assets (Removed to Prevent Cyclic Dependency)
# ------------------------------------------------------------------------
# The post-build asset copying is removed to avoid conflicting with pre-build steps
# If you prefer to keep asset copying here instead of pre-build, ensure that pre-build steps do not copy assets

# Remove or comment out the existing post-build asset copy to prevent duplication and potential cycles
# add_custom_command(
#     TARGET IliadEngine POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_directory
#     "${ASSETS_DIR}" "$<TARGET_FILE_DIR:IliadEngine>/Assets"
# )

# Additional CMake configurations for Vulkan (optional if needed)
if(WIN32)
    target_compile_definitions(IliadEngine PRIVATE VK_USE_PLATFORM_WIN32_KHR)
endif()

# Set working directory to the location of the executable (for Visual Studio .sln files)
set_property(TARGET IliadEngine PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:IliadEngine>")
