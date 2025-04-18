#include "Window.hpp"

xe::Window::Window(const std::string& windowName, float windowWidth, float windowHeight) :
                   name(windowName), width(windowWidth), height(windowHeight) {
    initWindow();
}

xe::Window::~Window() {
    glfwTerminate();
}

GLFWwindow* xe::Window::get() const {
    return window;
}

float xe::Window::getHeight() const {
    return height;
}

float xe::Window::getWidth() const {
    return width;
}

std::string xe::Window::getName() const {
    return name;
}

void xe::Window::initWindow() {
    if (!glfwInit()) {
        std::cerr << "GLFW init error" << std::endl;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
    if (!window) {
        std::cerr << "GLFW window creation error" << std::endl;
        glfwTerminate();
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
}

