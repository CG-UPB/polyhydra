#pragma once

#include "vospch.h"

#include "volumeshOS.h"

#include "WindowPanel.h"
#include "../settings/AppState.h"
#include "../util/Enums.h"
#include "rendering/camera/Camera.h"

namespace volumeshOS::Internal
{

    /**
     *  This class represents the toolbar. Various actions are possible in this toolbar. On the one hand there is the
     *  possibility to use the snapshot function to save snapshots via file dialog. Another possibility is the
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
         **/

        void show() override;
        void show(const std::shared_ptr<Internal::Camera>&);

    private:
        void show_screenshot_menu();

        static void show_general_menu();
        void show_selection_menu();
        void show_camera_menu();
        static void show_light_menu();
        static void show_post_processing_menu();
        static void show_sky_menu();

        static void show_shapes_menu();
        static void show_ground_menu();
        static void show_shadow_menu();
        static void show_ambient_occlusion_menu();
        static void show_transparency_menu();

        static void shift_right(float x = 30.0f);

    private:
        std::shared_ptr<Internal::Camera> m_camera  = nullptr;

        int m_manual_selection_type                 = 0;
        int m_manual_selection_id                   = -1;
        int m_previous_manual_selection_id          = -1;

        ExportOptions m_export_options              = {};
        int m_export_dimensions[2]                  = {m_export_options.width, m_export_options.height};
    };
}