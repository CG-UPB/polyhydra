#pragma once

#include "vospch.h"

#include "WindowPanel.h"

namespace volumeshOS::Internal
{

    class LogWindow final: public WindowPanel
    {
        public:

            void show() override;
            void add_message(const std::string& msg);

        private:

            std::vector<std::string> messages;
            bool m_visible = true;
            bool m_focused = false;
            bool m_adjust_scrollbar = false;
            float m_width = 250.0f;
            float m_height = 100.f;
            ImGuiDir m_arrow_dir = ImGuiDir_Up;

            static void show_message(const std::string& message);
            void switch_arrow();
    };
}