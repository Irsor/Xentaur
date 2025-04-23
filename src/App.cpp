#include "App.hpp"

xe::App::App(const std::string &appName) : name(appName) {
    Init();
}

xe::App::~App() {

}

void xe::App::Run() const {
    while (!glfwWindowShouldClose(window->get())) {
        glfwPollEvents();
    }
}

void xe::App::Init() {

    // Создание окна
    window = std::make_shared<xe::Window>(name);

    // Инициализация Vulkan
    core = std::make_unique<xe_core::Core>(name, window);
    numImages = core->GetNumImages();
    CreateCommandBuffers();
    RecordCommandBuffers();
}

void xe::App::CreateCommandBuffers() {
    commandBuffers.resize(numImages);
    core->CreateCommandBuffers(numImages, commandBuffers);
}

void xe::App::RecordCommandBuffers() {
    vk::ClearColorValue colorValue{ 0.0f, 1.0f, 0.0f, 1.0f };

    vk::ImageSubresourceRange imageRange{};
    imageRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
    imageRange.setLevelCount(1);
    imageRange.setLayerCount(1);

    for (size_t i = 0; i < commandBuffers.size(); i++) {
        try {
            vk::CommandBufferBeginInfo beginInfo{};

            commandBuffers[i]->begin(beginInfo);
            commandBuffers[i]->clearColorImage(core->GetImage(i), vk::ImageLayout::eGeneral, &colorValue, 1, &imageRange);
            commandBuffers[i]->end();
        }
        catch (const std::exception &ex) {
            std::cerr << "Failed to record commnad buffer: " << ex.what() << std::endl;
        }
    }
}
