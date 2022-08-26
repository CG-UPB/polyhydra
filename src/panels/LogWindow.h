#pragma once

#include "vospch.h"

#include "WindowPanel.h"


#ifndef stringify
#define stringify( name ) #name


namespace volumeshOS::Internal
{

    enum Type
    {
        Info,
        Warning,
        Error
    };

    constexpr const char* types[] =
    {
        "[Info]   ",
        "[Warning]",
        "[Error]  "
    };

    struct Message
    {
        time_t time;
        Type type;
        std::string msg;
    };

    class Renderer;

    class LogWindow
    {
        public:

            LogWindow();
            void show(float max_x, float max_y, const std::shared_ptr<Renderer>& renderer);
            void add_message(std::string msg, Type type);
            void clear_logs();
            void hide_log_window( bool hide);

        private:

            std::vector<Message> messages;
            static float min_height;
            ImGuiDir m_arrow_dir = ImGuiDir_Right;
            bool m_visible = false;
            bool m_focused = false;
            bool resizing = false;
            bool m_adjust_scrollbar = true;
            float m_width = 400.0f;
            float m_height = 100.f;
            float m_size_factor = 0.3;
            int max_msgs = 10;

            static void show_message(const Message& msg);
            void switch_arrow();

    };
}

#endif