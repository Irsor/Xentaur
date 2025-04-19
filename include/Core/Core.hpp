#pragma once

#include <iostream>
#include <vector>
#include <memory>

#include <vulkan/vulkan.hpp>
#include <glfw/glfw3.h>

#include "../Window.hpp"
#include "PhysicalDevices.hpp"

#ifndef NDEBUG
    const bool enableValidationLayers = true;
#else
    const bool enableValidationLayers = false;
#endif

namespace xe_core {
    class Core {
    public:
        Core(const std::string &appName, std::shared_ptr<xe::Window> window);
        ~Core();

    private:
        void Init(std::shared_ptr<xe::Window> window);
        void CreateInstance();
        std::vector<const char*> GetExtensions() const;
        void CreateSurface(std::shared_ptr<xe::Window> window);
        void GetPhysicalDevices();

        std::string name;
        vk::UniqueInstance instance;
        vk::UniqueSurfaceKHR surface;
        std::unique_ptr<PhysicalDevices> physicalDevices;
    };
}