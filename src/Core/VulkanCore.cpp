    #include "Core/VulkanCore.hpp"

xe_core::VulkanCore::VulkanCore(const std::string &appName, std::shared_ptr<xe::Window> window) : name(appName) {
    Init(window);
}

xe_core::VulkanCore::~VulkanCore() {

}

void xe_core::VulkanCore::Init(std::shared_ptr<xe::Window> window) {
    CreateInstance();
    CreateSurface(window);
}

void xe_core::VulkanCore::CreateInstance() {
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

std::vector<const char*> xe_core::VulkanCore::GetExtensions() const {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

void xe_core::VulkanCore::CreateSurface(std::shared_ptr<xe::Window> window) {
    VkSurfaceKHR vkSurface;
    if (glfwCreateWindowSurface(instance.get(), window->get(), nullptr, &vkSurface)) {
        throw std::runtime_error("Failed to create window surface!");
    }

    surface = vk::UniqueSurfaceKHR(vkSurface, instance.get());
}
