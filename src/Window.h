//
// Created by steffen on 06.10.21.
//

#ifndef VOLUMESHOS_WINDOW_H
#define VOLUMESHOS_WINDOW_H

#include <GLFW/glfw3.h>

#include <string>

namespace vOS
{
    class Window
    {

    public:
        Window(int width, int height, std::string title);
        void show();
        void destroy();

    private:

        int m_width;
        int m_height;
        std::string m_title;

        GLFWwindow* m_window;
    };
}

#endif //VOLUMESHOS_WINDOW_H
