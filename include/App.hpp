#pragma once

#include <iostream>
#include <memory>

#include <glfw/glfw3.h>

#include "Window.hpp"
#include "Core/VulkanCore.hpp"

namespace xe {
    class App {
    public:
        App(const std::string &appName);
        ~App();

        void Run() const;

    private:
        void Init();

        std::unique_ptr<xe::Window> window;
        std::unique_ptr<xe_core::VulkanCore> core;
        std::string name{};
    };
}