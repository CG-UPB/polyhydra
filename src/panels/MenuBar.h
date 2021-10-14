//
// Created by steffen on 12.10.21.
//

#ifndef VOLUMESHOS_MENUBAR_H
#define VOLUMESHOS_MENUBAR_H

#include "../Window.h"

namespace vOS
{
    class MenuBar: public WindowPanel
    {
        void show() override;
        ~MenuBar() override = default;
    };
}

#endif //VOLUMESHOS_MENUBAR_H
