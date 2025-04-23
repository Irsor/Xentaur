#include "App.hpp"

xe::App::App(const std::string &appName) : name(appName) {
    Init();
}

xe::App::~App() {

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
            // Создание барьера для подготовки изображения к очистке
            vk::ImageMemoryBarrier presentToClearBarrier{};
            presentToClearBarrier.setSrcAccessMask(vk::AccessFlagBits::eMemoryRead);
            presentToClearBarrier.setDstAccessMask(vk::AccessFlagBits::eTransferWrite);
            presentToClearBarrier.setOldLayout(vk::ImageLayout::eUndefined);
            presentToClearBarrier.setNewLayout(vk::ImageLayout::eTransferDstOptimal);
            presentToClearBarrier.setSrcQueueFamilyIndex(vk::QueueFamilyIgnored);
            presentToClearBarrier.setDstQueueFamilyIndex(vk::QueueFamilyIgnored);
            presentToClearBarrier.setImage(core->GetImage(i));
            presentToClearBarrier.setSubresourceRange(imageRange);

            // Создание барьера для подготовки изображения к отображению
            vk::ImageMemoryBarrier clearToPresentBarrier{};
            clearToPresentBarrier.setSrcAccessMask(vk::AccessFlagBits::eTransferRead);
            clearToPresentBarrier.setDstAccessMask(vk::AccessFlagBits::eMemoryRead);
            clearToPresentBarrier.setOldLayout(vk::ImageLayout::eTransferDstOptimal);
            clearToPresentBarrier.setNewLayout(vk::ImageLayout::ePresentSrcKHR);
            clearToPresentBarrier.setSrcQueueFamilyIndex(vk::QueueFamilyIgnored);
            clearToPresentBarrier.setDstQueueFamilyIndex(vk::QueueFamilyIgnored);
            clearToPresentBarrier.setImage(core->GetImage(i));
            clearToPresentBarrier.setSubresourceRange(imageRange);

            // Начало командного буфера
            vk::CommandBufferBeginInfo beginInfo{};
            beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eSimultaneousUse);

            commandBuffers[i]->begin(beginInfo);

            // Добавление барьера перед очисткой
            commandBuffers[i]->pipelineBarrier(
                vk::PipelineStageFlagBits::eTopOfPipe,
                vk::PipelineStageFlagBits::eTransfer,
                vk::DependencyFlags(),
                nullptr, nullptr, // Барьеры памяти не нужны
                presentToClearBarrier
            );

            // Очистка изображения
            commandBuffers[i]->clearColorImage(core->GetImage(i), vk::ImageLayout::eTransferDstOptimal, &colorValue, 1, &imageRange);

            // Добавление барьера после очистки, чтобы подготовить изображение для отображения
            commandBuffers[i]->pipelineBarrier(
                vk::PipelineStageFlagBits::eTransfer,
                vk::PipelineStageFlagBits::eBottomOfPipe,
                vk::DependencyFlags(),
                nullptr, nullptr, // Барьеры памяти не нужны
                clearToPresentBarrier
            );

            // Завершение командного буфера
            commandBuffers[i]->end();

        }
        catch (const std::exception &ex) {
            std::cerr << "Failed to record commnad buffer: " << ex.what() << std::endl;
        }
    }
}
