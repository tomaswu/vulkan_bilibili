cmake_minimum_required(VERSION 3.10)
project(DemoProject)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_PREFIX_PATH "C:/VulkanSDK/1.3.268.0")
list(APPEND CMAKE_PREFIX_PATH "E:/work_data/third_libs/vulkan")

find_program(GLSLC_PROGRAM glslc REQUIRED)
execute_process(COMMAND ${GLSLC_PROGRAM} ${CMAKE_CURRENT_SOURCE_DIR}/shader/render.frag -o ${CMAKE_CURRENT_SOURCE_DIR}/shader/render.frag.spv)
execute_process(COMMAND ${GLSLC_PROGRAM} ${CMAKE_CURRENT_SOURCE_DIR}/shader/render.vert -o ${CMAKE_CURRENT_SOURCE_DIR}/shader/render.vert.spv)

find_package(Vulkan REQUIRED)

file(GLOB srcs "srcs/*.cpp" "srcs/*.hpp")

add_executable(demo ${srcs})

target_include_directories(demo PRIVATE "E:/work_data/third_libs/sdl3/include")


target_link_libraries(demo PRIVATE 
    "E:/work_data/third_libs/sdl3/lib/mingw/libSDL3.a"
    winmm
    setupapi
    version
    imm32
    cfgmgr32
    Vulkan::Vulkan
)