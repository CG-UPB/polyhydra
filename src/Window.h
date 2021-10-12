//
// Created by steffen on 06.10.21.
//

#ifndef VOLUMESHOS_WINDOW_H
#define VOLUMESHOS_WINDOW_H

#include <GLFW/glfw3.h>

#include <string>
#include <vector>

namespace vOS
{
    class WindowPanel
    {
    public:
        virtual void show() = 0;
        virtual ~WindowPanel() = default;
    };

    class Window
    {

    public:
        Window(int width, int height, std::string title);
        ~Window();
        void show();

    private:

        void initPanels();

        int m_width;
        int m_height;
        std::string m_title;
        GLFWwindow* m_window;
        std::vector<WindowPanel*> m_panels;
    };
}

#endif //VOLUMESHOS_WINDOW_H
