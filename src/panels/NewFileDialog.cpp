//
// Created by projektgruppe on 26.01.22.
//

#include "NewFileDialog.h"
namespace vOS {

    NewFileDialog::NewFileDialog() {

    }

    char const *NewFileDialog::openDialog(const char * filedialog) {

        char const * lTheOpenFileName;
        lTheOpenFileName = tinyfd_openFileDialog(
                filedialog,
                "",
                1,
                l_OpenfilterPattern,
                NULL,
                0);

        if (! lTheOpenFileName)
        {
            tinyfd_messageBox(
                    "Error",
                    "Save file name is NULL",
                    "ok",
                    "error",
                    1);
            return NULL;
        }
        return lTheOpenFileName;
    }

    char const *NewFileDialog::saveDialog(const char * filedialog) {
        char const * lTheSaveFileName;
        lTheSaveFileName = tinyfd_saveFileDialog(
                filedialog,
                "passwordFile.txt",
                2,
                l_SavefilterPattern,
                NULL);

        if (! lTheSaveFileName)
        {
            tinyfd_messageBox(
                    "Error",
                    "Save file name is NULL",
                    "ok",
                    "error",
                    1);
            return NULL;
        }

        return lTheSaveFileName;
    }


}