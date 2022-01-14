#pragma once

#include "../ImguiRenderer.h"
#include "imgui.h"
#include <cstdarg>
#include <string.h>
#include <vector>
#include "../settings/GlobalViewerSettings.h"

namespace vOS
{
    class MeshLayerView final: public WindowPanel
    {
    public:
        ~MeshLayerView();
        static MeshLayerView* getInstance();
        void show() override;
    private:
        MeshLayerView();

        static MeshLayerView* instance;
    };
}