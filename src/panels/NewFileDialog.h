//
// Created by projektgruppe on 26.01.22.
//

#pragma once

#include "../ImguiRenderer.h"
#include "../filedialog/tinyfiledialogs.h"
#include "string.h"

namespace vOS {
    class NewFileDialog {
    public:
        NewFileDialog();
        ~NewFileDialog() = default;

        char const * openDialog(const char * filedialog);

    private:
        char const * lWillBeGraphicMode;
        char lBuffer[1024];
        char const * lFilterPatterns[1] = { "*.ovm" };
    };
}

