#pragma once

#include "vospch.h"

#include "WindowPanel.h"

#ifndef stringify
#define stringify( name ) #name

namespace volumeshOS::Internal
{


    struct Time
    {
       int hours;
       int minutes;
       int seconds;
    };

    enum Type
    {
        Info,
        Warning,
        Error
    };

    static const char* type_to_string[] =
    {
        stringify(Info),
        stringify(Warning),
        stringify(Error)
    };

    struct Message
    {
        Time time;
        Type type = Info;
        std::string msg;
    };

    class LogWindow
    {
        public:

            LogWindow();
            void show(float max_x, float max_y);
            void add_message(std::string msg, Type type = Info);
            void clear_logs();
            void hide_log_window( bool hide);

        private:

            std::vector<Message> messages;
            bool m_visible = true;
            bool m_focused = false;
            bool m_adjust_scrollbar = true;
            float m_width = 300.0f;
            float m_height = 100.f;
            ImGuiDir m_arrow_dir = ImGuiDir_Up;
            int max_msgs = 10;

            static void show_message(const Message& msg);
            void switch_arrow();

            Time get_time();
    };
}

#endif