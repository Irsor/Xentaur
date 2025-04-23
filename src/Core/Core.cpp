#include "Core/Core.hpp"

xe_core::Core::Core(const std::string &appName, std::shared_ptr<xe::Window> window) : name(appName) {
    Init(window);
}

xe_core::Core::~Core() {

}

unsigned int xe_core::Core::GetNumImages() const {
    return images.size();
}

vk::Image xe_core::Core::GetImage(unsigned int index) const {
    try {
        return images[index];
    } catch (const std::exception &ex) {
        std::cerr << "Image does not exists: " << ex.what() << std::endl;
    }
}

void xe_core::Core::CreateCommandBuffers(unsigned int numImages, std::vector<vk::UniqueCommandBuffer> &buffers) {
    buffers = AllocateCommandBuffers(numImages);
}

std::shared_ptr<xe_core::Queue> xe_core::Core::GetQueue() {
    return queue;
}

void xe_core::Core::Init(std::shared_ptr<xe::Window> window) {
    CreateInstance();
    CreateSurface(window);
    GetPhysicalDevices();
    CreateDevice();
    CreateSwapchain();
    CreateCommandPool();
    queue = std::make_shared<Queue>(device, swapchain, queueFamily, 0);
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
        queueFamily = physicalDevices->GetFamily();
    } catch (const std::exception &ex) {
        std::cerr << ex.what() << "Device creation error" << std::endl;
    }
}

void xe_core::Core::CreateSwapchain() {  
   auto selected = physicalDevices->GetSelected();  
   const auto &surfaceCapabilities = selected.surfaceCapabilities;  

   // Выбор количества изображений  
   int numImages = 0;  
   if ((surfaceCapabilities.maxImageCount > 0) && (surfaceCapabilities.minImageCount > surfaceCapabilities.maxImageCount)) {  
       numImages = surfaceCapabilities.maxImageCount;  
   } else {  
       numImages = surfaceCapabilities.minImageCount + 1;  
   }  

   // Выбор формата surface и пространства цвета  
   vk::SurfaceFormatKHR surfaceFormat = selected.surfaceFormats.front();  
   auto surfaceFormatIter = std::find_if(  
       selected.surfaceFormats.begin(),  
       selected.surfaceFormats.end(),  
       [](const vk::SurfaceFormatKHR &item) {  
           return item.format == vk::Format::eB8G8R8A8Srgb &&  
                  item.colorSpace == vk::ColorSpaceKHR::eVkColorspaceSrgbNonlinear;  
       }  
   );  

   surfaceFormat = surfaceFormatIter != selected.surfaceFormats.end() ? *surfaceFormatIter : selected.surfaceFormats.front();  

   // Выбор режима отображения  
   const auto& presentModes = selected.presentModes;  
   auto presentModeIter = std::find_if(  
       presentModes.begin(),  
       presentModes.end(),  
       [](const auto& item) {  
           return item == vk::PresentModeKHR::eMailbox;  
       }  
   );  

   auto presentMode = presentModeIter != presentModes.end() ? *presentModeIter : vk::PresentModeKHR::eFifo;  

   vk::SwapchainCreateInfoKHR swapchainCreateInfo{};  
   swapchainCreateInfo.surface = surface.get();  
   swapchainCreateInfo.imageFormat = surfaceFormat.format;  
   swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;  
   swapchainCreateInfo.imageExtent = surfaceCapabilities.currentExtent;  
   swapchainCreateInfo.minImageCount = numImages;  
   swapchainCreateInfo.imageArrayLayers = 1;  
   swapchainCreateInfo.imageUsage = (vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst);  
   swapchainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;  
   swapchainCreateInfo.queueFamilyIndexCount = 1;  
   swapchainCreateInfo.pQueueFamilyIndices = &queueFamily;  
   swapchainCreateInfo.preTransform = surfaceCapabilities.currentTransform;  
   swapchainCreateInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;  
   swapchainCreateInfo.presentMode = presentMode;  
   swapchainCreateInfo.clipped = vk::True;  

   try {  
       swapchain = device->createSwapchainKHRUnique(swapchainCreateInfo);  
       std::vector<vk::Image> swapchainImages = device->getSwapchainImagesKHR(swapchain.get());  

       for (const auto &image : swapchainImages) {
           vk::ImageViewCreateInfo imageViewCreateInfo{};  
           imageViewCreateInfo.setImage(image);
           imageViewCreateInfo.setViewType(vk::ImageViewType::e2D);
           imageViewCreateInfo.setFormat(surfaceFormat.format);
           imageViewCreateInfo.setComponents(vk::ComponentMapping(
               vk::ComponentSwizzle::eIdentity,
               vk::ComponentSwizzle::eIdentity,
               vk::ComponentSwizzle::eIdentity,
               vk::ComponentSwizzle::eIdentity
           ));
           imageViewCreateInfo.setSubresourceRange(vk::ImageSubresourceRange(
               vk::ImageAspectFlagBits::eColor,
               0, 1, 0, 1
           ));

           images.push_back(image);
           imageViews.push_back(device->createImageView(imageViewCreateInfo));
       }  
   } catch (const std::exception &ex) {
       std::cerr << "Failed to create Swapchain: " << ex.what() << std::endl;  
   }  
}

void xe_core::Core::CreateCommandPool() {
    vk::CommandPoolCreateInfo createInfo{};
    createInfo.setQueueFamilyIndex(queueFamily);

    try {
        commandPool = device->createCommandPoolUnique(createInfo);
    } catch (const std::exception &ex) {
        std::cerr << "Failed to create Command Pool: " << ex.what() << std::endl;
    }
}

std::vector<vk::UniqueCommandBuffer> xe_core::Core::AllocateCommandBuffers(unsigned int count) {
    vk::CommandBufferAllocateInfo allocateInfo{};
    allocateInfo.setCommandPool(commandPool.get());
    allocateInfo.setLevel(vk::CommandBufferLevel::ePrimary);
    allocateInfo.setCommandBufferCount(count);

    try {
        return device->allocateCommandBuffersUnique(allocateInfo);
    }
    catch (const std::exception &ex) {
        std::cerr << "Failed to allocate command buffers: " << ex.what() << std::endl;
        return {};
    }
}
