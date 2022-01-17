#pragma once

#include "../ImguiRenderer.h"
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

        //void setSelectedMeshes(std::vector<bool> sel){m_selected_meshs = sel};

    private:
        ToolBar();

        bool m_open_file = false;

        static ToolBar* instance;
        int m_timer_treshold = 3;

        bool m_selection_activated = false;
        int m_current_selection_mode;
        bool m_color_activated = true;
        float m_color[4] = {1.0f,1.0f,1.0f,1.0f};
        float m_slider_slicer = 0.0f;
        bool m_slicer_locked = false;
        int m_slider_peel = 0;
        bool m_isolation_started = false;
        int m_separation_type = 2;
        int m_lighting_type = 3;
        int m_rendering_mode = 0;
        float m_cell_size = 1.0f;
        int m_active_mesh = 0;

        bool showPopup1 = false;
        bool showPopup2 = false;

        bool test_selected[3] = {false,true,false};
    };
}