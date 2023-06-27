#pragma once

#include "vospch.h"

#include "WindowPanel.h"
#include "../settings/AppState.h"

namespace volumeshOS::Internal
{
    /**
     * This class represents the UI-Prompt, which shows a Mesh List. It shows some options for every single mesh, that
     * is shown in the viewport. The name of the meshes is there unique ID
     */
    class MeshLayerView final: public WindowPanel
    {
    public:

        /**
         * This method creates a UI window that lists the individual meshes. Each currently loaded mesh is listed with
         * its ID. In addition, various options can be applied to the individual meshes via buttons. On the one hand,
         * each mesh has a radio button that sets the active mesh and sets a mesh in focus by double-clicking it.
         * In addition, a checkbox can be used to decide whether a mesh is displayed or not. A color select button sets
         * the color of the mesh and a combo sets the rendering mode.
         */
        void show() override;

    private:

        void render_popup(const VMesh& mesh);

        void render_cells_popup(const VMesh& mesh);

        void render_lines_popup(const VMesh& mesh);

        void render_points_popup(const VMesh& mesh);

        void render_mesh_options(const VMesh& mesh);

        void render_mesh_settings(const VMesh& mesh);

        static bool render_header(const VMesh& mesh);

        float m_slider_slicer                       = 0.0f;
        bool m_slicer_locked                        = false;
        float m_slider_peel                         = 0;
        bool m_digging_activated                    = false;
        bool m_isolation_started                    = false;
        float m_cell_size                           = 1.0f;
        float m_mesh_position[3]                    = {0.0f, 0.0f, 0.0f};
        float m_mesh_scale                          = 1.0f;
        float m_mesh_rotation[3]                    = {0.0f, 0.0f, 0.0f};
        float m_last_rotation[3]                    = {0.0f, 0.0f, 0.0f};
    };
}