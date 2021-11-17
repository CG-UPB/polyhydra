#pragma once

#include "../Window.h"
#include "imgui.h"
#include <cstdarg>
#include <string.h>
#include <vector>

namespace vOS
{
    class ToolBox: public WindowPanel
    {
        public:
            ToolBox();
            ~ToolBox();

            void show() override;

        private:
            bool m_vertex_selection;
    };
}