add_executable(engine
    "src/main.cpp"
    "src/Window/Window.cpp"
    "include/Window/Window.hpp"
    "src/App/App.cpp"
)

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

set_property(TARGET engine PROPERTY CXX_STANDARD 23)