#pragma once

#include "vospch.h"

#include "WindowPanel.h"

namespace volumeshOS::Internal
{

    class LogWindow
    {
        public:

            LogWindow();
            void show(float max_x, float max_y);
            void add_message(const std::string& msg);

        private:

            std::vector<std::string> messages;
            bool m_visible = true;
            bool m_focused = false;
            bool m_adjust_scrollbar = true;
            float m_width = 300.0f;
            float m_height = 100.f;
            ImGuiDir m_arrow_dir = ImGuiDir_Up;
            int max_msgs = 10;

            static void show_message(const std::string& message);
            void switch_arrow();
    };
}