//
// Created by steffen on 12.10.21.
//

#ifndef VOLUMESHOS_INPUT_H
#define VOLUMESHOS_INPUT_H

class GLFWwindow;

namespace vOS
{
    class Input
    {
    public:

        // glfw callbacks
        static void glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void glfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        static void glfwMouseCursorPosCallback(GLFWwindow* window, double xpos, double ypos);
        static void glfwScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

        // reset the current scroll offset for the next frame
        static void resetOffset();

        // should be self explanatory
        static bool isKeyDown(int key);
        static double getMouseX();
        static double getMouseY();
        static double getScrollOffsetX();
        static double getScrollOffsetY();

    private:
        Input();
        ~Input();
        static Input& getInstance();

        // map for each key code as index, true if currently pressed
        bool* m_keyMap;

        double m_currentMouseX;
        double m_currentMouseY;

        double m_currentScrollOffsetX;
        double m_currentScrollOffsetY;
    };
}

#endif //VOLUMESHOS_INPUT_H
