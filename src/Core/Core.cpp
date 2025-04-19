#include "Core/Core.hpp"

xe_core::Core::Core(const std::string &appName, std::shared_ptr<xe::Window> window) : name(appName) {
    Init(window);
}

xe_core::Core::~Core() {

}

void xe_core::Core::Init(std::shared_ptr<xe::Window> window) {
    CreateInstance();
    CreateSurface(window);
    GetPhysicalDevices();
    CreateDevice();
}

void xe_core::Core::CreateInstance() {
    std::vector<const char*> layers = {
            "VK_LAYER_KHRONOS_validation"
    };

    vk::ApplicationInfo appInfo{};
    appInfo.pApplicationName = name.c_str(),
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
    appInfo.pEngineName = "No Engine",
    appInfo.engineVersion = vk::makeApiVersion(1, 4, 312, 0),
    appInfo.apiVersion = VK_MAKE_VERSION(1, 4, 0);

    vk::InstanceCreateInfo createInfo{};
    createInfo.flags = vk::InstanceCreateFlags(0);
    createInfo.pApplicationInfo = &appInfo;
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = layers.size();
        createInfo.ppEnabledLayerNames = layers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }
    auto extensions = GetExtensions();
    createInfo.enabledExtensionCount = extensions.size();
    createInfo.ppEnabledExtensionNames = extensions.data();

    try {
        instance = vk::createInstanceUnique(createInfo);
    } catch (const std::exception &ex) {
        std::cerr << "Failed to create Vulkan instance: " << ex.what() << std::endl;
    }
}

std::vector<const char*> xe_core::Core::GetExtensions() const {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

void xe_core::Core::CreateSurface(std::shared_ptr<xe::Window> window) {
    VkSurfaceKHR vkSurface;
    if (glfwCreateWindowSurface(instance.get(), window->get(), nullptr, &vkSurface)) {
        throw std::runtime_error("Failed to create window surface!");
    }

    surface = vk::UniqueSurfaceKHR(vkSurface, instance.get());
}

void xe_core::Core::GetPhysicalDevices() {
    physicalDevices = std::make_unique<xe_core::PhysicalDevices>(instance, surface);
}

void xe_core::Core::CreateDevice() {
    float queuePriorities[] = { 1.0f };

    vk::DeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.queueFamilyIndex = physicalDevices->GetFamily();
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriorities[0];

    std::vector<const char*> extensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME
    };

    if (physicalDevices->GetSelected().features.geometryShader == vk::False) {
        throw std::exception("Geometry shader is not supported");
    }

    if (physicalDevices->GetSelected().features.tessellationShader == vk::False) {
        throw std::exception("Tesselation shader is not supported");
    }

    vk::PhysicalDeviceFeatures features{};
    features.geometryShader = vk::True;
    features.tessellationShader = vk::True;

    vk::DeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    deviceCreateInfo.enabledExtensionCount = extensions.size();
    deviceCreateInfo.ppEnabledExtensionNames = extensions.data();
    deviceCreateInfo.pEnabledFeatures = &features;

    try {
        device = physicalDevices->GetSelected().device.createDeviceUnique(deviceCreateInfo);
    } catch (const std::exception &ex) {
        std::cerr << ex.what() << "Device creation error" << std::endl;
    }
}