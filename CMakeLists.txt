cmake_minimum_required(VERSION 3.18)

set(pname vkbilibili)
message(name:: ${pname})

project(${pname})

set(CMAKE_CXX_STANDARD 17)

set(CXX_STANDARD_REQUIRED ON)

cmake_host_system_information(RESULT _host_name QUERY HOSTNAME)
message("-- HOSTNAME environment variable is: " ${_host_name})

if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")

    # set(Qt6_DIR /Users/tomaswu/Qt/6.6.0/macos/lib/cmake/Qt6)
    # find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
    # set(glfw_include "/opt/homebrew/Cellar/glfw/3.4/include")
    # set(glfw_libs "/opt/homebrew/Cellar/glfw/3.4/lib")
    set(Vulkan_DIR "~/VulkanSDK/1.3.280.1/macOS/lib/cmake")
    set(glm_include  "/opt/homebrew/Cellar/glm/1.0.1/include")
    set(glm_libs "/opt/homebrew/Cellar/glm/1.0.1/lib")
    set(SDL2_ROOT /opt/homebrew/Cellar/sdl2/2.30.2/lib/cmake)

elseif(_host_name STREQUAL "DESKTOP-JN9PKOL")
    set(glfw_include "D:/data/third_libs/glfw3/include")
    set(glfw_libs "D:/data/third_libs/glfw3/lib-vc2022")
    set(Vulkan_DIR "C:/VulkanSDK/1.3.268.0/cmake")

endif()

find_package(Vulkan REQUIRED)
message("vulkan found?::" ${Vulkan_FOUND})

message("vulkan include path::" ${Vulkan_INCLUDE_DIRS})

find_package(SDL2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS} ${Vulkan_INCLUDE_DIRS} ${glm_include})
link_directories(${glm_libs})

file(GLOB srcs "*.cpp" "*.hpp")

add_executable(${pname} ${srcs})

target_link_libraries(${pname} PRIVATE SDL2::SDL2 glm Vulkan::Vulkan)


