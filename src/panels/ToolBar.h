#pragma once

#include "vospch.h"

#include "WindowPanel.h"
#include "../settings/AppState.h"
#include "../util/Enums.h"

namespace volumeshOS::Internal
{

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
         * This method is used to draw the toolbar in every loop. It creates all the buttons and sets the corresponding
         * variables
         */
        void show() override;

    private:
        float slider_width = 180.0f;
        float padding_right = 20.0f;

        int m_current_selection_mode = static_cast<int>(SelectionMode::OFF);
        int m_manual_selection_type = 0;
        int m_previous_manual_selection_type = 0;
        int m_manual_selection_id = -1;
        int m_previous_manual_selection_id = -1;
        float m_slider_slicer = 0.0f;
        bool m_slicer_locked = false;
        float m_slider_peel = 0;
        bool m_digging_activated = false;
        bool m_isolation_started = false;
        float m_cell_size = 1.0f;
        int m_active_mesh = 0;
        float m_mesh_position[3] = {0.0f, 0.0f, 0.0f};
        float m_mesh_scale = 1.0f;
        float m_mesh_rotation[3] = {0.0f, 0.0f, 0.0f};


        void show_rendering_mode_menu();
        void show_selection_menu();
        void show_camera_menu();

        void show_ground_menu();
        void show_shadow_menu();
        void show_ambient_occlusion_menu();
        void show_transparency_menu();

        void show_mesh_list();
        void shift_right();
    };
}