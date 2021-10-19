#pragma once

#include "../Window.h"
#include "imgui.h"
#include <cstdarg>

namespace vOS
{
    class LogWindow
    {
        public:
            LogWindow();
            void clear();
            void addLog(const char* fmt, ...) IM_FMTARGS(2);
            void draw(const char* title, bool* p_open = NULL);
            ImGuiTextBuffer Buf;
            ImGuiTextFilter filter;
            ImVector<int>   lineOffsets;
        private:
            bool autoScroll;
    };
}