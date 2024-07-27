#include <functional>
#include <vulkan/vulkan.hpp>


using CreateSurfaceFunc = std::function<vk::SurfaceKHR(vk::Instance)>;

std::string ReadModuleFile(const std::string &path);