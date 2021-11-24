#pragma once

#include "../Window.h"
#include "imgui.h"
#include <cstdarg>
#include <string.h>
#include <vector>

namespace vOS
{
    class Test final: public WindowPanel
    {
    public:
        ~Test();
        static Test* getInstance();
        void clear();
        void addLog(const char* fmt, int level = 0);
        void addLog(std::string fmt, int level = 0);
        void show() override;
        ImGuiTextBuffer Buf;
        ImGuiTextFilter filter;
        ImVector<int>   lineOffsets;
        std::string levels[4];
    private:
        Test();

        static Test* instance;
        bool autoScroll;
        std::vector<ImVec4> colors;
        bool m_vertex_selection;
        bool m_edge_selection;
        int m_current_selection_mode;
        int m_timer_treshold = 3;
        float m_color;
    };
}