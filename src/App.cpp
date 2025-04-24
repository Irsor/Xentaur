#include "App.hpp"

xe::App::App(const std::string &appName) : name(appName) {
    Init();
}

xe::App::~App() {
    core->DestroyRenderPass(renderPass);
}

void xe::App::Run() const {
    while (!glfwWindowShouldClose(window->get())) {
        unsigned int imageIndex = queue->AcquireNextImage();
        queue->SubmitAsync(commandBuffers[imageIndex]);
        queue->Present(imageIndex);
        glfwPollEvents();
    }
}

void xe::App::Init() {

    // Создание окна
    window = std::make_shared<xe::Window>(name);

    // Инициализация Vulkan
    core = std::make_unique<xe_core::Core>(name, window);
    numImages = core->GetNumImages();
    queue = core->GetQueue();
    renderPass = core->CreateDefaultRenderPass();
    frameBuffers = core->CreateFrameBuffers(renderPass);
    CreateCommandBuffers();
    RecordCommandBuffers();
}

void xe::App::CreateCommandBuffers() {
    commandBuffers.resize(numImages);
    core->CreateCommandBuffers(numImages, commandBuffers);
}

void xe::App::RecordCommandBuffers() {
    vk::ClearColorValue colorValue{ 0.0f, 1.0f, 0.0f, 1.0f };
    vk::ClearValue clearValue{};
    clearValue.setColor(colorValue);

    vk::RenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.setRenderPass(renderPass);
    renderPassBeginInfo.setRenderArea({
        { 0, 0 },
        { static_cast<uint32_t>(window->getWidth()), static_cast<uint32_t>(window->getHeight()) },
    });
    renderPassBeginInfo.setClearValueCount(1);
    renderPassBeginInfo.setPClearValues(&clearValue);

    for (size_t i = 0; i < commandBuffers.size(); i++) {
        try {
         
            // Начало командного буфера
            vk::CommandBufferBeginInfo beginInfo{};
            commandBuffers[i].begin(beginInfo);

            beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eSimultaneousUse);

            renderPassBeginInfo.setFramebuffer(frameBuffers[i]);
            
            commandBuffers[i].beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
            commandBuffers[i].endRenderPass();

            // Завершение командного буфера
            commandBuffers[i].end();

        }
        catch (const std::exception &ex) {
            std::cerr << "Failed to record commnad buffer: " << ex.what() << std::endl;
        }
    }
}
