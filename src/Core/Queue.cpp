#include "Core/Queue.hpp"

xe_core::Queue::Queue(const vk::UniqueDevice &vulkanDevice, const vk::UniqueSwapchainKHR &vulkanSwapchain,
                      int queueFamily, int queueIndex) : device(vulkanDevice.get()), swapchain(vulkanSwapchain.get()) {
    device.getQueue(queueFamily, queueIndex, &queue);
    Init();
}

xe_core::Queue::~Queue() {}

void xe_core::Queue::Init() {
    CreateSemaphores();
}

int xe_core::Queue::AcquireNextImage() {
    uint32_t imageIndex = 0;
    try {
        vk::Result result = device.acquireNextImageKHR(
            swapchain,
            UINT64_MAX,
            presentCompleteSemaphore.get(),
            nullptr,
            &imageIndex
        );
        if (result != vk::Result::eSuccess) {
            throw std::runtime_error("Failed to acquire next image");
        }
    } catch (const std::exception& ex) {
        std::cerr << "Failed to acquire next image: " << ex.what() << std::endl;
    }

    return imageIndex;
}

void xe_core::Queue::Submit(const vk::CommandBuffer &commandBuffer) {
    vk::SubmitInfo submitInfo{};
    submitInfo.setCommandBufferCount(1);
    submitInfo.setPCommandBuffers(&commandBuffer);

    try {
        queue.submit(submitInfo);
    } catch (const std::exception &ex) {
        std::cerr << "Failed to Submit Async: " << ex.what() << std::endl;
    }
}

void xe_core::Queue::SubmitAsync(const vk::CommandBuffer &commandBuffer) {
    vk::PipelineStageFlags flags = vk::PipelineStageFlagBits::eColorAttachmentOutput;

    vk::SubmitInfo submitInfo{};
    submitInfo.setWaitSemaphoreCount(1);
    submitInfo.setPWaitSemaphores(&presentCompleteSemaphore.get());
    submitInfo.setPWaitDstStageMask(&flags);
    submitInfo.setCommandBufferCount(1);
    submitInfo.setPCommandBuffers(&commandBuffer);
    submitInfo.setSignalSemaphoreCount(1);
    submitInfo.setPSignalSemaphores(&renderCompleteSemaphore.get());

    try {
        queue.submit(submitInfo);
    } catch (const std::exception &ex) {
        std::cerr << "Failed to Submit Async: " << ex.what() << std::endl;
    }
}

void xe_core::Queue::Present(const unsigned int imageIndex) {
    vk::PresentInfoKHR presentInfo{};
    presentInfo.setWaitSemaphoreCount(1);
    presentInfo.setPWaitSemaphores(&renderCompleteSemaphore.get());
    presentInfo.setSwapchainCount(1);
    presentInfo.setPSwapchains(&swapchain);
    presentInfo.setPImageIndices(&imageIndex);

    try {
        vk::Result result = queue.presentKHR(&presentInfo);
        if (result != vk::Result::eSuccess) {
            throw std::runtime_error("Failed to Present");
        }
    } catch (const std::exception& ex) {
        std::cerr << "Failed to Present: " << ex.what() << std::endl;
    }

    Wait();
}

void xe_core::Queue::Wait() {
    queue.waitIdle();
}

void xe_core::Queue::CreateSemaphores() {
    vk::SemaphoreCreateInfo renderSemaphoreCreateInfo{};
    vk::SemaphoreCreateInfo presentSemaphoreCreateInfo{};

    try {
        renderCompleteSemaphore = device.createSemaphoreUnique(renderSemaphoreCreateInfo);
        presentCompleteSemaphore = device.createSemaphoreUnique(presentSemaphoreCreateInfo);
    } catch (const std::exception &ex) {
        std::cerr << "Failed to create queue semaphores: " << ex.what() << std::endl;
    }
}
