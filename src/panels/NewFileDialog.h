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
        char const * saveDialog(const char * filedialog);

    private:
        char const * lWillBeGraphicMode;
        char lBuffer[1024];
        char const * l_OpenfilterPattern[1] = {"*.ovm" };
        char const * l_SavefilterPattern[2] = {"*.png", "*.bmp" };
    };
}

