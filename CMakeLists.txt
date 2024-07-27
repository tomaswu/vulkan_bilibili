cmake_minimum_required(VERSION 3.28)

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
    link_directories(${glm_libs})
    find_package(SDL2 REQUIRED)
elseif(_host_name STREQUAL "DESKTOP-JN9PKOL")
    list(APPEND CMAKE_PREFIX_PATH "H:/lib3rd/glm-1.0.1-light/glm")
    set(glm_include "H:/lib3rd/glm-1.0.1-light/glm")
    set(Vulkan_DIR "C:/VulkanSDK/1.3.268.0/cmake")
    set(SDL2_INCLUDE_DIRS "H:/lib3rd/SDL2-devel-2.26.5-VC/SDL2-2.26.5/include")
    link_directories("H:/lib3rd/SDL2-devel-2.26.5-VC/SDL2-2.26.5/lib/x64")
endif()

#*****************
find_program(GLSLC_PROGRAM glslc REQUIRED)
execute_process(COMMAND ${GLSLC_PROGRAM} ${CMAKE_SOURCE_DIR}/shader/shader.vert -o ${CMAKE_SOURCE_DIR}/shader/vert.spv)
execute_process(COMMAND ${GLSLC_PROGRAM} ${CMAKE_SOURCE_DIR}/shader/shader.frag -o ${CMAKE_SOURCE_DIR}/shader/frag.spv)
file(COPY ${CMAKE_SOURCE_DIR}/shader/vert.spv ${CMAKE_SOURCE_DIR}/shader/frag.spv DESTINATION ${CMAKE_BINARY_DIR})

#*****************

find_package(Vulkan REQUIRED)
message("vulkan found?::" ${Vulkan_FOUND})

message("vulkan include path::" ${Vulkan_INCLUDE_DIRS})

include_directories(${SDL2_INCLUDE_DIRS} ${Vulkan_INCLUDE_DIRS} ${glm_include})

file(GLOB srcs "*.cpp" "*.hpp")

add_executable(${pname} ${srcs})
if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    target_link_libraries(${pname} PRIVATE SDL2::SDL2 glm Vulkan::Vulkan)
elseif(_host_name STREQUAL "DESKTOP-JN9PKOL")
    target_link_libraries(${pname} PRIVATE SDL2 Vulkan::Vulkan)
endif()











