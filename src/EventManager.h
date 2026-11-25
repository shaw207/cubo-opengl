#pragma once
#include "common.h"
using namespace std;

class EventManager
{
public:
    static inline map<int, bool> keyMap;
    static inline GLFWwindow* window = nullptr;

    static inline double mouseX = 0.0;
    static inline double mouseY = 0.0;
    static inline double deltaMX = 0.0;
    static inline double deltaMY = 0.0;
    static inline bool firstMouseSample = true;

    static void initEventManager(GLFWwindow* window);

    static void keyboardManager(GLFWwindow* window, int key, int scancode,
        int action, int mods);

    static void mouseMoveManager(GLFWwindow* window, double xpos, double ypos);

    static void updateEvents();
};
