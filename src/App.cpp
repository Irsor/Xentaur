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
}