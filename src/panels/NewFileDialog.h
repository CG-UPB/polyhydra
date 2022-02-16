//
// Created by projektgruppe on 26.01.22.
//

#pragma once

#include "../ImguiRenderer.h"
#include "../filedialog/tinyfiledialogs.h"

namespace vOS {

    /**
     * This class represents a file dialog. The user can use the native file dialog of the operating system. This class
     * has methods to open a file - as result an OVM file is returned, and to save a file - as result a png or bmp file
     * is returned.
     */
    class NewFileDialog {
    public:

        /**
         * creates a new Object of the class, which could be used to create a file dialog
         */
        NewFileDialog();
        ~NewFileDialog() = default;

        /**
         *
         * This method creates a file dialog, which returns the path of an ovm-file, after the user has chosen some. If
         * the user cancels the Input, or gives a wrong file ending an error-prompt will be displayed and return value will be "NULL"
         *
         * @param filedialog - the title of the window prompt
         * @return char const * filename, or NULL
         */
        char const * openDialog(const char * filedialog);

        /**
         *
         * This method creates a file dialog, which returns the path of an png- or bmp-file, after the user has chosen some. If
         * the user cancels the Input, or gives a wrong file ending an error-prompt will be displayed and return value will be "NULL"
         *
         * @param filedialog - the title of the window prompt
         * @return char const * filename, or NULL
         */
        char const * saveDialog(const char * filedialog);

    private:

        // Buffer which is used for input of filename
        char lBuffer[1024];
        // file ending for the openfile-dialog
        char const * l_OpenfilterPattern[1] = {"*.ovm" };
        // file endings for the savefile-dialog
        char const * l_SavefilterPattern[2] = {"*.png", "*.bmp" };
    };
}

