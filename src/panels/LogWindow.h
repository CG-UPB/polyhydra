#pragma once

#include "../Window.h"
#include "imgui.h"
#include <cstdarg>

namespace vOS
{
    class LogWindow: public WindowPanel
    {
        public:
            LogWindow();
            ~LogWindow() override;
            void clear();
            void addLog(const char* fmt, ...) IM_FMTARGS(2);
            void show() override;
            ImGuiTextBuffer Buf;
            ImGuiTextFilter filter;
            ImVector<int>   lineOffsets;
        private:
            bool autoScroll;
    };
}