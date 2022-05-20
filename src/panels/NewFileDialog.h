#pragma once

#include "../ImguiRenderer.h"
#include <tinyfiledialogs.h>

namespace vOS
{

    /**
     * This class represents a file dialog. The user can use the native file dialog of the operating system. This class
     * has methods to open a file - as result an OVM file is returned, and to save a file - as result a png or bmp file
     * is returned.
     */
    class NewFileDialog
    {
    public:

        /**
         * creates a new Object of the class, which could be used to create a file dialog
         */
        NewFileDialog();

        ~NewFileDialog() = default;

        /**
         *
         * This method creates a file dialog, which returns the path of a ovm-file, after the user has chosen some. If
         * the user cancels the Input, or gives a wrong file ending an error-prompt will be displayed and return value will be "NULL"
         *
         * @param filedialog - the title of the window prompt
         * @return char const * filename, or NULL
         */
        char const* openDialog(const char* filedialog);

        /**
         *
         * This method creates a file dialog, which returns the path of a png- or bmp-file, after the user has chosen some. If
         * the user cancels the Input, or gives a wrong file ending an error-prompt will be displayed and return value will be "NULL"
         *
         * @param filedialog - the title of the window prompt
         * @param default_name - the default name of the file
         * @return char const * filename, or NULL
         */
        char const* saveDialog(const char* filedialog, std::string default_name = "file.png");

        /**
         *
         * This method creates a file dialog, which returns the path of a json file, after the user has chosen some. If
         * the user cancels the Input, or gives a wrong file ending an error-prompt will be displayed and return value will be "NULL"
         *
         * @param filedialog - the title of the window prompt
         * @return char const * filename, or NULL
         */
        char const* loadMeshSettings(const char* filedialog);

        /**
         *
         * This method creates a file dialog, which returns the path of a json-file, after the user has chosen some. If
         * the user cancels the Input, or gives a wrong file ending an error-prompt will be displayed and return value will be "NULL"
         *
         * @param filedialog - the title of the window prompt
         * @return char const * filename, or NULL
         */
        char const* saveMeshSettings(const char* filedialog);

    private:

        // Buffer which is used for input of filename
        char lBuffer[1024];
        // file ending for the openfile-dialog
        char const* l_OpenfilterPattern[1] = {"*.ovm"};
        // file endings for the savefile-dialog
        char const* l_SavefilterPattern[2] = {"*.png", "*.bmp"};

        // file ending for the load-mesh-data-dialog
        char const* l_LoadMeshfilterPattern[1] = {"*.json"};
        // file ending for the save-mesh-data-dialog
        char const* l_SaveMeshfilterPattern[1] = {"*.json"};
    };
}

