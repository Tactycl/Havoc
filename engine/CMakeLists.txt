# Collect all source and header files
file(GLOB_RECURSE ENGINE_SOURCES
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
)

add_executable(engine ${ENGINE_SOURCES})

# GLFW integration
include(FetchContent)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.9
)

FetchContent_MakeAvailable(glfw)

# Vulkan integration
find_package(Vulkan REQUIRED)

target_link_libraries(engine
    PRIVATE
        glfw
        Vulkan::Vulkan
)

target_include_directories(engine PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

set_property(TARGET engine PROPERTY CXX_STANDARD 23)