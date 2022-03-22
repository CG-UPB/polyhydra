
#include "Input.h"
#include "GLFW/glfw3.h"

namespace vOS
{
    // Static Variables
    bool Input::m_ignore_keyboard_commands;
    bool Input::m_accept_inputs;
    bool Input::m_ignore_mouse_commands;
    glm::vec3 Input::m_movement_vector;
    glm::vec2 Input::m_scroll_offset;
    std::map<int, int> Input::m_keybinds;
    GLFWwindow* Input::m_window;

    void Input::setup(GLFWwindow* window)
    {
        m_window = window;

        m_movement_vector = {0.0f, 0.0f, 0.0f};
        m_scroll_offset = {0.0f, 0.0f};

        // Setup
        accept_input(true);
        // We may want to read from a persistent Data file in the future

        // Assign Callbacks
        glfwSetScrollCallback(m_window, Input::glw_callback_mouse_scroll);
    }

    void Input::accept_input(bool accept)
    {
        m_accept_inputs = accept;
    }

    void Input::cleanup()
    {

    }

    void Input::ignore_keyboard(bool ignore)
    {
        m_ignore_keyboard_commands = ignore;
    }

    void Input::ignore_mouse(bool ignore)
    {
        m_ignore_mouse_commands = ignore;
    }

    void Input::set_keybind(int from, int to)
    {
        // Erase Whatever keybinds exist that contain <from> or <to>
        if (m_keybinds.find(from) != m_keybinds.end())
            m_keybinds.erase(from);
        if (m_keybinds.find(to) != m_keybinds.end())
            m_keybinds.erase(to);

        // Insert a two-way bind between <from> and <to>
        m_keybinds.insert(std::pair<int, int>(to, from));
        m_keybinds.insert(std::pair<int, int>(from, to));
    }

    void Input::reset_keybinds()
    {
        m_keybinds.clear();
    }

    int Input::get_key_bind(int key)
    {
        return m_keybinds.find(key) != m_keybinds.end() ? m_keybinds[key] : key;
    }

    bool Input::key_pressed(int imgui_key_id)
    {
        return ImGui::IsKeyDown(get_key_bind(imgui_key_id));
    }

    void Input::update()
    {
        if (!(m_accept_inputs) || m_ignore_keyboard_commands)
            return;

        // use ImGui callbacks instead of glfw, since glfw callbacks are not called every frame and cause movement lags
        glm::vec3 mov(0.0f);
        mov.x += ImGui::IsKeyDown(get_key_bind(GLFW_KEY_D)) ? 1.0f : 0.0f;
        mov.x -= ImGui::IsKeyDown(get_key_bind(GLFW_KEY_A)) ? 1.0f : 0.0f;
        mov.y += ImGui::IsKeyDown(get_key_bind(GLFW_KEY_SPACE)) ? 1.0f : 0.0f;
        mov.y -= ImGui::IsKeyDown(get_key_bind(GLFW_KEY_LEFT_SHIFT)) ? 1.0f : 0.0f;
        mov.z += ImGui::IsKeyDown(get_key_bind(GLFW_KEY_W)) ? 1.0f : 0.0f;
        mov.z -= ImGui::IsKeyDown(get_key_bind(GLFW_KEY_S)) ? 1.0f : 0.0f;
        if (mov.x != 0.0f || mov.y != 0.0f || mov.z != 0.0f)
        {
            mov = glm::normalize(mov);
        }
        m_movement_vector = mov;
    }

    void Input::glw_callback_mouse_scroll(GLFWwindow* window, double xoffset, double yoffset)
    {
        if (!m_accept_inputs || m_ignore_mouse_commands)
            return;

        m_scroll_offset = {xoffset, yoffset};
    }

    void Input::reset_offset()
    {
        m_scroll_offset = {0.0f, 0.0f};
    }


    // use the ImGui methods for this, since the glfw callback is not called every frame, which causes lags while moving the mouse
    bool Input::mouse_pressed()
    { return ImGui::IsMouseDown(ImGuiMouseButton_Left); }

    double Input::get_mouse_X()
    { return ImGui::GetMousePos().x; }

    double Input::get_mouse_Y()
    { return ImGui::GetMousePos().y; }

    bool Input::controll_pressed()
    {
        return key_pressed(GLFW_KEY_LEFT_CONTROL);
    }

    float Input::get_wasd_movement_vector_X()
    { return m_movement_vector.x; }

    float Input::get_wasd_movement_vector_Y()
    { return m_movement_vector.y; }

    float Input::get_wasd_movement_vector_Z()
    { return m_movement_vector.z; }

    double Input::get_scroll_offset_X()
    { return m_scroll_offset.x; }

    double Input::get_scroll_offset_Y()
    { return m_scroll_offset.y; }
}