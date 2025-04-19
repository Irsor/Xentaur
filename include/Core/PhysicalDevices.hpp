#pragma once

#include <vector>
#include <memory>
#include <iostream>

#include <vulkan/vulkan.hpp>

namespace xe_core {
    struct PhysicalDevice {
        vk::PhysicalDevice device;
        vk::PhysicalDeviceProperties deviceProperties;
        std::vector<vk::QueueFamilyProperties2> queueFamilyProperties;
        std::vector<vk::Bool32> queueSupportsPresent;
        std::vector<vk::SurfaceFormatKHR> surfaceFormats;
        vk::SurfaceCapabilitiesKHR surfaceCapabilities;
        vk::PhysicalDeviceMemoryProperties memoryProperties;
        std::vector<vk::PresentModeKHR> presentModes;
    };

    class PhysicalDevices {
    public:
        PhysicalDevices(const vk::UniqueInstance &instance, const vk::UniqueSurfaceKHR &surface);
        ~PhysicalDevices();

        void SelectDevice(vk::QueueFlags requiredQueueType, bool supportsPresent);
        const PhysicalDevice &GetSelected() const;
        int GetFamily() const;

    private:
        void Init(const vk::UniqueInstance &instance, const vk::UniqueSurfaceKHR &surface);

        std::vector<PhysicalDevice> devices{};
        unsigned int queueFamily{};
        int deviceIndex{-1};
    };
}