#pragma once

#include "../Window.h"

namespace vOS
{
    class MenuBar: public WindowPanel
    {
        void show() override;
        ~MenuBar() override = default;
    };
}