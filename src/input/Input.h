#import <map>
#include <functional>

class GLFWwindow;

namespace vOS {
    class Input {
    public:

        static void setup(GLFWwindow* window);
        static void cleanup();

        static void set_keybind(int from, int to);
        static void reset_keybinds();
        static void accept_input(bool accept);

        static void ignore_keyboard(bool ignore);
        static void ignore_mouse(bool ignore);

        static bool mouse_pressed();

        static double get_mouse_X();

        static double get_mouse_Y();

        static double get_scroll_offset_X();

        static double get_scroll_offset_Y();

        // reset the current scroll offset for the next frame
        static void reset_offset();
    private:
        // Variables
        static bool m_accept_inputs;
        static bool m_ignore_keyboard_commands;
        static bool m_ignore_mouse_commands;
        static double m_currentMouseX;
        static double m_currentMouseY;

        static bool m_mouse_pressed;

        static double m_currentScrollOffsetX;
        static double m_currentScrollOffsetY;

        // Bindings
        static std::map<int,int> m_keybinds;

        // References
        static GLFWwindow* m_window;

        // Callback Functions
        static void call_function(int key);

        static void glfw_callback_key(GLFWwindow *window, int key, int scancode, int action, int mods);
        static void glw_callback_mouse_button(GLFWwindow *window, int button, int action, int mods);
        static void glw_callback_mouse_position(GLFWwindow *window, double xpos, double ypos);
        static void glw_callback_mouse_scroll(GLFWwindow *window, double xoffset, double yoffset);
    };
}

/*
#pragma once

class GLFWwindow;

namespace vOS
{
    class Input
    {
    public:

        // glfw callbacks
        static void glfwKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

        static void glfwMouseButtonCallback(GLFWwindow *window, int button, int action, int mods);

        static void glfwMouseCursorPosCallback(GLFWwindow *window, double xpos, double ypos);

        static void glfwScrollCallback(GLFWwindow *window, double xoffset, double yoffset);

        // reset the current scroll offset for the next frame
        static void resetOffset();

        // should be self explanatory
        static bool isKeyDown(int key);

        static bool isKeyReleased(int key);

        static double getMouseX();

        static double getMouseY();

        static double getScrollOffsetX();

        static double getScrollOffsetY();

    private:
        Input();

        ~Input();

        static Input &getInstance();

        // map for each key code as index, true if currently pressed
        bool *m_keyMap;
        bool *m_keyReleaseMap;

        double m_currentMouseX;
        double m_currentMouseY;

        double m_currentScrollOffsetX;
        double m_currentScrollOffsetY;
    };
}*/