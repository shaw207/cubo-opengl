#include "EventManager.h"

void EventManager::initEventManager(GLFWwindow* window)
{
    glfwSetKeyCallback(window, keyboardManager);
    glfwSetCursorPosCallback(window, mouseMoveManager);
    EventManager::window = window;
    EventManager::firstMouseSample = true;
    EventManager::deltaMX = 0.0;
    EventManager::deltaMY = 0.0;
}

void EventManager::keyboardManager(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    switch (action) {
    case GLFW_PRESS:
        EventManager::keyMap[key] = true;
        break;
    case GLFW_RELEASE:
        EventManager::keyMap[key] = false;
        break;
    }
}

void EventManager::mouseMoveManager(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouseSample) {
        mouseX = xpos;
        mouseY = ypos;
        deltaMX = 0.0;
        deltaMY = 0.0;
        firstMouseSample = false;
        return;
    }

    deltaMX = xpos - mouseX;
    deltaMY = ypos - mouseY;
    mouseX = xpos;
    mouseY = ypos;
}

void EventManager::updateEvents()
{
    EventManager::deltaMX = 0.0;
    EventManager::deltaMY = 0.0;

    glfwPollEvents();
}
