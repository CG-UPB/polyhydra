#pragma once

#include "../ImguiRenderer.h"
#include "imgui.h"
#include <cstdarg>
#include <string.h>
#include <vector>

namespace vOS
{
    /**
     * This class represents a LogWindow, which is shown in vOS and where the User could log some messages. It is
     * realised as Singleton
     */
    class LogWindow final: public WindowPanel
    {
        public:
            ~LogWindow();
            /**
             * static Singleton method to get the class-object
             * @return the class-object
             */
            static LogWindow* getInstance();

            /**
             * clears the LogWindow - deletes all thats in the buffer
             */
            void clear();
            /**
             * adds a Message to the LogWindow. The level specifies the criticality/topic of the message. Available levels are:
             *
             * 0 -> information
             * 1 -> warning
             * 2 -> error
             * 3 -> critical
             *
             * @param fmt const char * which should be written on the LogWindwo
             * @param level criticality of the message
             */
            void addLog(const char* fmt, int level = 0);

            /**
             * adds a Message to the LogWindow. The level specifies the criticality/topic of the message. Available levels are:
             *
             * 0 -> information
             * 1 -> warning
             * 2 -> error
             * 3 -> critical
             *
             * @param fmt std::string which should be written on the LogWindwo
             * @param level criticality of the message
             */
            void addLog(std::string fmt, int level = 0);

            /**
             *  This method is used to draw the LogWindow in every loop. It creates all the buttons and the lines of text
             * variables
             */
            void show() override;

        private:
            /**
             * private Constructor for the Singleton
             */
            LogWindow();

            // this variable holds the class-object for the Singleton
            static LogWindow* instance;

            // Variables that are used for Buffering and filtering
            ImGuiTextBuffer Buf;
            ImGuiTextFilter filter;
            ImVector<int>   lineOffsets;
            std::string levels[4];

            // autoscroll is a feature that could be activated
            bool autoScroll;
            // not working: but the message should be written in colors - bugging with autoscroll feature
            std::vector<ImVec4> colors;
    };
}