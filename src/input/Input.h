#include <map>
#include <functional>

class GLFWwindow;

namespace vOS {
    /*
     * This class fetches GLFW Keyboard and Mouse Inputs, translates them if keybinds are set, and operates on different classes depending on the input
     */
    class Input {
    public:

        /*
         * Setup and Construction
         */
        static void setup(GLFWwindow* window);
        /*
         * Cleanup and Destruction
         */
        static void cleanup();

        /*
         * Everytime <from> is pressed, <to> will be called
         */
        static void set_keybind(int from, int to);
        /*
         * Deletes all set Keybinds
         */
        static void reset_keybinds();
        /*
         * If set to false, no button or mouse input will be read
         */
        static void accept_input(bool accept);

        /*
         * if set to true, button input will be ignored
         */
        static void ignore_keyboard(bool ignore);
        /*
         * if set to true, keyboard input will be ignored
         */
        static void ignore_mouse(bool ignore);

        /*
         * True if left mouse button is pressed
         */
        static bool mouse_pressed();

        /*
         * X coordinate of Mouse
         */
        static double get_mouse_X();

        /*
         * Y coordinate of Mouse
         */
        static double get_mouse_Y();

        /*
         * X Scroll offset of Mouse Wheel
         */
        static double get_scroll_offset_X();

        /*
         * Y Scroll offset of Mouse Wheel
         */
        static double get_scroll_offset_Y();

        /*
         * Movement vector X direction
         */
        static float get_wasd_movement_vector_X();
        /*
         * Movement vector Y direction
         */
        static float get_wasd_movement_vector_Y();
        /*
         * Movement vector Z direction
         */
        static float get_wasd_movement_vector_Z();

        // reset the current scroll offset for the next frame
        static void reset_offset();
    private:
        // Variables

        // Input acceptance
        static bool m_accept_inputs;
        static bool m_ignore_keyboard_commands;
        static bool m_ignore_mouse_commands;

        // Movement Vector
        static float m_movement_vector_x;
        static float m_movement_vector_y;
        static float m_movement_vector_z;


        // Mouse Position
        static double m_currentMouseX;
        static double m_currentMouseY;

        // Mouse Button presses
        static bool m_mouse_pressed;

        // Mouse Scroll Offset
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