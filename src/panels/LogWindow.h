#pragma once

#include "../Window.h"
#include "imgui.h"
#include <cstdarg>
#include <string.h>
#include <vector>

namespace vOS
{
    class LogWindow final: public WindowPanel
    {
        public:
            ~LogWindow();
            static LogWindow* getInstance();
            void clear();
            void addLog(const char* fmt, int level = 0);
            void show() override;
            ImGuiTextBuffer Buf;
            ImGuiTextFilter filter;
            ImVector<int>   lineOffsets;
            std::string levels[4];
        private:
            LogWindow();

            static LogWindow* instance;
            bool autoScroll;
            std::vector<ImVec4> colors;
    };
}