#pragma once

#include <iostream>
#include <vector>

#include <vulkan/vulkan.hpp>
#include <glfw/glfw3.h>

#ifndef NDEBUG
    const bool enableValidationLayers = true;
#else
    const bool enableValidationLayers = false;
#endif

namespace xe_core {
    class VulkanCore {
    public:
        VulkanCore(const std::string &appName);
        ~VulkanCore();

    private:
        void Init();
        void CreateInstance();
        std::vector<const char*> GetExtensions() const;

        std::string name;
        vk::UniqueInstance instance;
    };
}