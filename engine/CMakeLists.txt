# =========================
# Engine Target
# =========================

cmake_minimum_required(VERSION 3.20)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)

project(engine LANGUAGES CXX)

file(GLOB_RECURSE ENGINE_SOURCES
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

add_executable(engine ${ENGINE_SOURCES})

set_property(TARGET engine PROPERTY CXX_STANDARD 23)
set_target_properties(engine PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:engine>
)

if(APPLE)
    target_compile_definitions(engine PRIVATE HAVOC_VK_USE_PORTABILITY)
endif()

# =========================
# Dependencies
# =========================

include(FetchContent)

set(FETCHCONTENT_QUIET ON)
set(CMAKE_MESSAGE_LOG_LEVEL "ERROR")
set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS ON CACHE BOOL "" FORCE)

# -------------------------
# GLFW
# -------------------------
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.9
)
FetchContent_MakeAvailable(glfw)

# -------------------------
# GLM
# -------------------------
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
)
FetchContent_MakeAvailable(glm)

# -------------------------
# ShaderC
# -------------------------
FetchContent_Declare(
    spirv_headers
    GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Headers.git
    GIT_TAG vulkan-sdk-1.3.290.0
)

FetchContent_MakeAvailable(spirv_headers)

FetchContent_Declare(
    spirv-tools
    GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Tools.git
    GIT_TAG v2024.1
)

FetchContent_MakeAvailable(spirv-tools)

FetchContent_Declare(
    glslang
    GIT_REPOSITORY https://github.com/KhronosGroup/glslang.git
    GIT_TAG 14.3.0
)

set(ENABLE_SPVREMAPPER OFF CACHE BOOL "" FORCE)
set(ENABLE_OPT OFF CACHE BOOL "" FORCE)
set(ENABLE_GLSLANG_BINARIES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glslang)

set(SHADERC_ENABLE_SHARED_CRT OFF CACHE BOOL "" FORCE)
set(SHADERC_ENABLE_WERROR OFF CACHE BOOL "" FORCE)
set(SHADERC_SKIP_TESTS ON CACHE BOOL "" FORCE)
set(SHADERC_SKIP_EXAMPLES ON CACHE BOOL "" FORCE)

FetchContent_Declare(
    shaderc
    GIT_REPOSITORY https://github.com/google/shaderc.git
    GIT_TAG v2024.1
)

FetchContent_MakeAvailable(shaderc)

# =========================
# Vulkan
# =========================
find_package(Vulkan REQUIRED)

if(NOT TARGET Vulkan::Vulkan)
    message(FATAL_ERROR "Vulkan target not found")
endif()

# =========================
# Linking
# =========================

target_link_libraries(engine
    PRIVATE
        glfw
        Vulkan::Vulkan
        glm::glm
        shaderc_combined
)

target_include_directories(engine SYSTEM PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${Vulkan_INCLUDE_DIRS}
)

# =========================
# Windows runtime fix
# =========================

if (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()

add_custom_command(TARGET engine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/engine/shaders
    $<TARGET_FILE_DIR:engine>/shaders
)