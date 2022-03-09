#include "GLFW/glfw3.h"
#include "Input.h"
#include <functional>
#include "../Window.h"

namespace vOS
{
    // Static Variables
    bool Input::m_ignore_keyboard_commands;
    bool Input::m_accept_inputs;
    bool Input::m_ignore_mouse_commands;
    float Input::m_movement_vector_x;
    float Input::m_movement_vector_y;
    float Input::m_movement_vector_z;
    double Input::m_currentMouseX;
    double Input::m_currentMouseY;
    bool Input::m_mouse_pressed;
    double Input::m_currentScrollOffsetX;
    double Input::m_currentScrollOffsetY;
    std::map<int,int> Input::m_keybinds;
    GLFWwindow* Input::m_window;

    void Input::setup(GLFWwindow* window)
    {
        m_window = window;

        m_movement_vector_x = 0;
        m_movement_vector_y = 0;
        m_movement_vector_z = 0;

          m_currentScrollOffsetX = 0;
          m_currentScrollOffsetY = 0;
        m_mouse_pressed = false;

        // Setup
        accept_input(true);
        // We may want to read from a persistent Data file in the future

        // Assign Callbacks
        glfwSetKeyCallback(m_window, Input::glfw_callback_key);
        glfwSetMouseButtonCallback(m_window, Input::glw_callback_mouse_button);
        glfwSetCursorPosCallback(m_window, Input::glw_callback_mouse_position);
        glfwSetScrollCallback(m_window, Input::glw_callback_mouse_scroll);
    }

    void Input::accept_input(bool accept) {
        m_accept_inputs = accept;
    }

    void Input::cleanup() {

    }

    void Input::ignore_keyboard(bool ignore) {
        m_ignore_keyboard_commands = ignore;
    }

    void Input::ignore_mouse(bool ignore) {
        m_ignore_mouse_commands = ignore;
    }

    void Input::set_keybind(int from, int to) {
        // Erase Whatever keybinds exist that contain <from> or <to>
        if(m_keybinds.find(from) != m_keybinds.end())
            m_keybinds.erase(from);
        if(m_keybinds.find(to) != m_keybinds.end())
            m_keybinds.erase(to);

        // Insert a two-way bind between <from> and <to>
        m_keybinds.insert(std::pair<int,int>(to,from) );
        m_keybinds.insert(std::pair<int,int>(from,to) );
    }

    void Input::reset_keybinds() {
        m_keybinds.clear();
    }

    void Input::glfw_callback_key(GLFWwindow *window, int key, int scancode, int action, int mods){
        if(! ( m_accept_inputs) || m_ignore_keyboard_commands)
            return;

        // Rebind if an entry in our rebind map is found
        int rebind = m_keybinds.find(key) != m_keybinds.end() ? m_keybinds[key] : key;

        action = action > 1 ? 1: action;

        float x = m_movement_vector_x;
        float y = m_movement_vector_y;
        float z = m_movement_vector_z;

        // Camera Movement
        // Occupied Letters: W, A, S, D, SPACE, LEFT_SHIFT
        if(rebind == GLFW_KEY_D)
            x = action;
        else if(rebind == GLFW_KEY_A)
            x = -action;
        if(rebind == GLFW_KEY_SPACE)
            y = action;
        else if(rebind == GLFW_KEY_LEFT_SHIFT)
            y = -action;
        if(rebind == GLFW_KEY_W)
            z = action;
        else if(rebind == GLFW_KEY_S)
            z = -action;

        // Single Action Keys ( only trigger when key is pressed )
        if(action == GLFW_PRESS ) {

            // Do Action Depending on Input Key
            // Rendering Mode Switches
            // Occupied Letters: I, O, P, L
            if (rebind == GLFW_KEY_I) {
                //Window::instance().set_mesh_rendering_mode("mesh_phong");
            } else if (rebind == GLFW_KEY_O) {
//                Window::instance().set_mesh_rendering_mode("mesh_wireframe");
            } else if (rebind == GLFW_KEY_P) {
//                Window::instance().set_mesh_rendering_mode("mesh_flat");
            } else if (rebind == GLFW_KEY_L) {
//                Window::instance().set_mesh_rendering_mode("mesh_normal");
            }
        }

        glm::vec3 mov = {x, y, z};

        if(mov.x != 0 || mov.y != 0 || mov.z != 0) {
            mov = glm::normalize(mov);
        }
        m_movement_vector_x = mov.x;
        m_movement_vector_y = mov.y;
        m_movement_vector_z = mov.z;
    }

    void Input::glw_callback_mouse_button(GLFWwindow *window, int button, int action, int mods){
        if(!m_accept_inputs)
            return;

        m_mouse_pressed = button == 0 && action == GLFW_PRESS;
    }

    void Input::glw_callback_mouse_position(GLFWwindow *window, double xpos, double ypos){
        if(!m_accept_inputs || m_ignore_mouse_commands)
            return;

        m_currentMouseX = xpos;
        m_currentMouseY = ypos;
    }
    void Input::glw_callback_mouse_scroll(GLFWwindow *window, double xoffset, double yoffset){

        if(!m_accept_inputs || m_ignore_mouse_commands)
            return;

        m_currentScrollOffsetX = xoffset;
        m_currentScrollOffsetY = yoffset;
    }

    void Input::reset_offset()
    {
        m_currentScrollOffsetX = 0.0;
        m_currentScrollOffsetY = 0.0;
    }

     bool Input::mouse_pressed(){return m_mouse_pressed;}

     double Input::get_mouse_X(){return m_currentMouseX;}

     double Input::get_mouse_Y(){return m_currentMouseY;}

    float Input::get_wasd_movement_vector_X() { return m_movement_vector_x;}
    float Input::get_wasd_movement_vector_Y() { return m_movement_vector_y;}
    float Input::get_wasd_movement_vector_Z() { return m_movement_vector_z;}

     double Input::get_scroll_offset_X(){return m_currentScrollOffsetX;}

     double Input::get_scroll_offset_Y(){return m_currentScrollOffsetY;}
}
/*
#include "GLFW/glfw3.h"
#include "Input.h"

namespace vOS
{

    Input& Input::getInstance()
    {
        static Input instance;
        return instance;
    }

    Input::Input(): m_currentMouseX(0.0), m_currentMouseY(0.0), m_currentScrollOffsetX(0.0), m_currentScrollOffsetY(0.0)
    {
        m_keyMap = new bool[65536] {false};
        m_keyReleaseMap = new bool[65536] {false};
    }

    Input::~Input()
    {
        delete m_keyMap;
        delete m_keyReleaseMap;
    }

    void Input::glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if (key >= 0 && key < 65536)
        {
            Input::getInstance().m_keyMap[key] = action == GLFW_PRESS;
            if (action == GLFW_RELEASE)
            {
                Input::getInstance().m_keyReleaseMap[key] = !Input::getInstance().m_keyReleaseMap[key];

            }
        }
    }

    void Input::glfwMouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
    {
        if (button >= 0 && button < 65536)
        {
            Input::getInstance().m_keyMap[button] = action == GLFW_PRESS;
            Input::getInstance().m_keyReleaseMap[button] = action == GLFW_RELEASE;
        }
    }

    void Input::glfwMouseCursorPosCallback(GLFWwindow *window, double xpos, double ypos)
    {
        Input::getInstance().m_currentMouseX = xpos;
        Input::getInstance().m_currentMouseY = ypos;
    }

    void Input::glfwScrollCallback(GLFWwindow *window, double xoffset, double yoffset)
    {
        Input::getInstance().m_currentScrollOffsetX = xoffset;
        Input::getInstance().m_currentScrollOffsetY = yoffset;
    }

    bool Input::isKeyDown(int key)
    {
        return Input::getInstance().m_keyMap[key];

    }

    bool Input::isKeyReleased(int key)
    {
        return Input::getInstance().m_keyReleaseMap[key];
    }

    double Input::getMouseX()
    {
        return Input::getInstance().m_currentMouseX;
    }

    double Input::getMouseY()
    {
        return Input::getInstance().m_currentMouseY;
    }

    double Input::getScrollOffsetX()
    {
        return Input::getInstance().m_currentScrollOffsetX;
    }

    double Input::getScrollOffsetY()
    {
        return Input::getInstance().m_currentScrollOffsetY;
    }

    void Input::resetOffset()
    {
        Input::getInstance().m_currentScrollOffsetX = 0.0;
        Input::getInstance().m_currentScrollOffsetY = 0.0;
    }
}*/