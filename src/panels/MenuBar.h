#pragma once

#include "../ImguiRenderer.h"
namespace vOS
{
    class MenuBar: public WindowPanel
    {
    public:
        void show() override;
        ~MenuBar() override = default;

    };
}