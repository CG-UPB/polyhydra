//
// Created by projektgruppe on 26.01.22.
//

#include "NewFileDialog.h"
namespace vOS {

    NewFileDialog::NewFileDialog() {
        // empty constructor
    }

    char const *NewFileDialog::openDialog(const char * filedialog) {

        // Variable for the filename
        char const * lTheOpenFileName;

        // opens the open file dialog - for more information see https://github.com/native-toolkit/tinyfiledialogs
        lTheOpenFileName = tinyfd_openFileDialog(
                filedialog,
                "",
                1,
                l_OpenfilterPattern,
                NULL,
                0);

        // error message if the file dialog was used wrong
        if (! lTheOpenFileName)
        {
            tinyfd_messageBox(
                    "Info",
                    "No Filename received",
                    "ok",
                    "info",
                    1);
            return NULL;
        }
        return lTheOpenFileName;
    }

    char const *NewFileDialog::saveDialog(const char * filedialog) {
        char const * lTheSaveFileName;
        // opens the save file dialog - for more information see https://github.com/native-toolkit/tinyfiledialogs
        lTheSaveFileName = tinyfd_saveFileDialog(
                filedialog,
                "file.png",
                2,
                l_SavefilterPattern,
                NULL);

        // error message if the file dialog was used wrong
        if (! lTheSaveFileName)
        {
            tinyfd_messageBox(
                    "Information",
                    "No Filename received",
                    "ok",
                    "info",
                    1);
            return NULL;
        }

        return lTheSaveFileName;
    }


}