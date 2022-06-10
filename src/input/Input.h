#pragma once

#include "vospch.h"
#include "GLFW/glfw3.h"

class GLFWwindow;

namespace vOS {
    /**
     * This class fetches GLFW Keyboard and Mouse Inputs, translates them if keybinds are set, and operates on different classes depending on the input
     */
    class Input {
    public:

        enum
        {
            SWITCH_CAMERA_MODE = GLFW_KEY_M,
            ROTATE_MESH = GLFW_KEY_LEFT_SHIFT,
            TRANSLATE_MESH = GLFW_KEY_LEFT_CONTROL
        };

        /**
         * Setup and Construction
         */
        static void setup(GLFWwindow* window);
        /**
         * Cleanup and Destruction
         */
        static void cleanup();

        /**
         * Returns true if given imgui key is pressed
         * Takes rebinds into consideration
         * @param imgui_key_id
         * @return
         */
        static bool key_pressed(int imgui_key_id);

        static bool key_down(int imgui_key_id);
        /**
         * Everytime <from> is pressed, <to> will be called
         */
        static void set_keybind(int from, int to);
        /**
         * Deletes all set Keybinds
         */
        static void reset_keybinds();
        /**
         * If set to false, no button or mouse input will be read
         */
        static void accept_input(bool accept);

        /**
         * if set to true, button input will be ignored
         */
        static void ignore_keyboard(bool ignore);
        /**
         * if set to true, keyboard input will be ignored
         */
        static void ignore_mouse(bool ignore);

        /**
         * True if left mouse button is pressed
         */
        static bool mouse_pressed();

        static bool mouse_double_clicked();

        static glm::vec2 get_mouse_coords();

        static glm::vec2 get_scroll_offset();

        static glm::vec3 get_wasd_movement_vector();

        // reset the current scroll offset for the next frame
        static void reset_offset();

        static void update();

    private:
        // Variables

        // Input acceptance
        static bool m_accept_inputs;
        static bool m_ignore_keyboard_commands;
        static bool m_ignore_mouse_commands;

        // Movement Vector
        static glm::vec3 m_movement_vector;

        // Mouse Scroll Offset
        static glm::vec2 m_scroll_offset;

        // Bindings
        static std::map<int,int> m_keybinds;

        // References
        static GLFWwindow* m_window;

        // Callback Functions
        static void call_function(int key);

        static void glw_callback_mouse_scroll(GLFWwindow *window, double xoffset, double yoffset);

        static int get_key_bind(int key);
    };
}