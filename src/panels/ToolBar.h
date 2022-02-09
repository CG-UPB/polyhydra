#pragma once

#include "../ImguiRenderer.h"
#include "imgui.h"
#include <cstdarg>
#include <string.h>
#include <vector>
#include "../settings/GlobalViewerSettings.h"

namespace vOS
{
    // Enum for better understandability
    enum Selection{
        Vertex = 1,
        Edge = 2,
        Face = 3
    };

    /**
     *  This class represents the toolbar. Various actions are possible in this toolbar. On the one hand there is the
     *  possibility to use the snapshot function to save snapshots via a file dialog. Another possibility is the
     *  activation of the selection feature, through which the individual elements of the meshes can be selected. It is
     *  also possible to specify whether only vertices, edges, faces or cells can be selected. Below this, on the
     *  toolbar, is the tab for the mesh settings, which are always displayed and applied to the active mesh.
     */
    class ToolBar final: public WindowPanel
    {
    public:
        /**
         * creates an instance of the toolbar
         */
        ToolBar();
        ~ToolBar();
        /**
         * This method is used to draw the toolbar in every loop. It creates all the buttons and sets the corresponding
         * variables
         */
        void show() override;

    private:


        bool m_open_file = false;

        bool m_selection_activated = false;
        int m_current_selection_mode;
        int m_manual_selection_type = 0;
        int m_previous_manual_selection_type = 0;
        int m_manual_selection_id = -1;
        int m_previous_manual_selection_id = -1;
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
        float m_mesh_position[3] = {0.0f, 0.0f, 0.0f};
        float m_mesh_scale = 1.0f;

        bool test_selected[3] = {false,true,false};
    };
}