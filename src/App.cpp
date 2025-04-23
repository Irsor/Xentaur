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
}

void xe::App::CreateCommandBuffers() {
    commandBuffers.resize(numImages);
    core->CreateCommandBuffers(numImages, commandBuffers);

    for (const auto& item : commandBuffers) {
        std::cout << item.get() << std::endl;
    }
}
