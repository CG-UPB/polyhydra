#pragma once

#include "vospch.h"

#include "WindowPanel.h"
#include "../settings/GlobalViewerSettings.h"

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
         * creates an instance of the class
         */
        MeshLayerView();
        ~MeshLayerView();

        /**
         * This method creates a UI window that lists the individual meshes. Each currently loaded mesh is listed with
         * its ID. In addition, various options can be applied to the individual meshes via buttons. On the one hand,
         * each mesh has a radio button that sets the active mesh and sets a mesh in focus by double-clicking it.
         * In addition, a checkbox can be used to decide whether a mesh is displayed or not. A color select button sets
         * the color of the mesh and a combo sets the rendering mode.
         */
        void show() override;
    private:
        // timer_treshold for the tooltipps
        int m_timer_treshold = 3;
    };
}