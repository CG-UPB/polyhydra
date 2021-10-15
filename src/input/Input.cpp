
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
    }

    Input::~Input()
    {
        delete m_keyMap;
    }

    void Input::glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if (key >= 0 && key < 65536)
        {
            Input::getInstance().m_keyMap[key] = action == GLFW_PRESS;
        }
    }

    void Input::glfwMouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
    {
        if (button >= 0 && button < 65536)
        {
            Input::getInstance().m_keyMap[button] = action == GLFW_PRESS;
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
}