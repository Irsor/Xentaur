#include <iostream>

#include "Window.hpp"

int main() {
    xe::Window window;

    while (!glfwWindowShouldClose(window.get())) {
        glfwPollEvents();
    }

    return 0;
}
