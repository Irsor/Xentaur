#include "Core/PhysicalDevices.hpp"

xe_core::PhysicalDevices::PhysicalDevices(const vk::UniqueInstance &instance,
                                          const vk::UniqueSurfaceKHR &surface) {
    Init(instance, surface);
    SelectDevice(vk::QueueFlagBits::eGraphics, true);
}

xe_core::PhysicalDevices::~PhysicalDevices() {

}

void xe_core::PhysicalDevices::SelectDevice(vk::QueueFlags requiredQueueType, bool supportsPresent) {
    for (int i = 0; i < devices.size(); i++) {
        for (int j = 0; devices[i].queueFamilyProperties.size(); j++) {
            const auto &properties = devices[i].queueFamilyProperties[j].queueFamilyProperties;
            if (properties.queueFlags && static_cast<bool>(devices[i].queueSupportsPresent[j]) == supportsPresent) {
                deviceIndex = i;
                queueFamily = j;
                break;
            }
        }
    }

    if (deviceIndex == -1) {
        throw std::exception("Required device not found");
    }
}

void xe_core::PhysicalDevices::Init(const vk::UniqueInstance &instance, const vk::UniqueSurfaceKHR &surface) {
    std::vector<vk::PhysicalDevice> physicalDevices = instance->enumeratePhysicalDevices();

    if (physicalDevices.empty()) {
        throw std::runtime_error("There is no devices");
    }

    for (const auto &physicalDevice : physicalDevices) {
        PhysicalDevice device{};
        device.device = physicalDevice;
        device.deviceProperties = physicalDevice.getProperties();
        device.queueFamilyProperties = physicalDevice.getQueueFamilyProperties2();
        for (int i = 0; i < device.queueFamilyProperties.size(); i++) {
            device.queueSupportsPresent.push_back(physicalDevice.getSurfaceSupportKHR(i, surface.get(), surface.getDispatch()));
        }
        device.surfaceFormats = physicalDevice.getSurfaceFormatsKHR(surface.get(), surface.getDispatch());
        device.surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface.get(), surface.getDispatch());
        device.presentModes = physicalDevice.getSurfacePresentModesKHR(surface.get(), surface.getDispatch());
        device.memoryProperties = physicalDevice.getMemoryProperties();

        devices.push_back(device);
    }
}

const xe_core::PhysicalDevice &xe_core::PhysicalDevices::GetSelected() const {
    return devices[deviceIndex];
}

int xe_core::PhysicalDevices::GetFamily() const {
    return queueFamily;
}