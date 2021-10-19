#pragma once

#include "../Window.h"
#include "imgui.h"
#include <cstdarg>

namespace vOS
{
    class LogWindow final: public WindowPanel
    {
        public:
            ~LogWindow();
            static LogWindow* getInstance();
            void clear();
            void addLog(const char* fmt, ...) IM_FMTARGS(2);
            void show() override;
            ImGuiTextBuffer Buf;
            ImGuiTextFilter filter;
            ImVector<int>   lineOffsets;
        private:
            LogWindow();

            static LogWindow* instance;
            bool autoScroll;
    };
}