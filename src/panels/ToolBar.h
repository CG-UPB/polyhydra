#pragma once

#include "../Window.h"
#include "imgui.h"
#include <cstdarg>
#include <string.h>
#include <vector>
#include "../settings/GlobalViewerSettings.h"

namespace vOS
{
    enum Selection{
        Vertex = 0,
        Edge = 1,
        Face = 2
    };

    class ToolBar final: public WindowPanel
    {
    public:
        ~ToolBar();
        static ToolBar* getInstance();
        void show() override;

    private:
        ToolBar();

        static ToolBar* instance;
        int m_timer_treshold = 3;

        int m_current_selection_mode;
        float m_color = 0.5f;
        float m_slider_slicer = 0.5f;
        float m_slider_peel = 0.5f;
        bool m_isolation_started = false;
        int m_separation_type = 2;
        int m_lighting_type = 3;
    };
}