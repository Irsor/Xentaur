#pragma once

#include <iostream>
#include <memory>

#include <glfw/glfw3.h>
#include <vulkan/vulkan.hpp>

#include "Window.hpp"
#include "Core/Core.hpp"

namespace xe {
    class App {
    public:
        App(const std::string &appName);
        ~App();

        void Run() const;

    private:
        void Init();

        void CreateCommandBuffers();
        void RecordCommandBuffers();

        std::shared_ptr<xe::Window> window;
        std::string name{};

        // Render
        std::unique_ptr<xe_core::Core> core;
        int numImages{0};
        std::vector<vk::UniqueCommandBuffer> commandBuffers{};
        std::shared_ptr<xe_core::Queue> queue;
    };
}