
#include "NewFileDialog.h"

namespace volumeshOS
{

    NewFileDialog::NewFileDialog()
    {
        // empty constructor
    }

    char const* NewFileDialog::openDialog(const char* filedialog)
    {

        // Variable for the filename
        char const* lTheOpenFileName;

        // opens the open file dialog - for more information see https://github.com/native-toolkit/tinyfiledialogs
        lTheOpenFileName = tinyfd_openFileDialog(
                filedialog,
                "",
                1,
                l_OpenfilterPattern,
                NULL,
                0);

        // error message if the file dialog was used wrong
        if (!lTheOpenFileName)
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

    char const* NewFileDialog::saveDialog(const char* filedialog, std::string default_name)
    {
        char const* lTheSaveFileName;
        // opens the save file dialog - for more information see https://github.com/native-toolkit/tinyfiledialogs
        lTheSaveFileName = tinyfd_saveFileDialog(
                filedialog,
                default_name.c_str(),
                2,
                l_SavefilterPattern,
                NULL);

        // error message if the file dialog was used wrong
        if (!lTheSaveFileName)
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


    char const* NewFileDialog::loadMeshSettings(const char* filedialog)
    {

        // Variable for the filename
        char const* lTheOpenFileName;

        // opens the open file dialog - for more information see https://github.com/native-toolkit/tinyfiledialogs
        lTheOpenFileName = tinyfd_openFileDialog(
                filedialog,
                "",
                1,
                l_LoadMeshfilterPattern,
                NULL,
                0);

        // error message if the file dialog was used wrong
        if (!lTheOpenFileName)
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

    char const* NewFileDialog::saveMeshSettings(const char* filedialog)
    {
        char const* lTheSaveFileName;
        // opens the save file dialog - for more information see https://github.com/native-toolkit/tinyfiledialogs
        lTheSaveFileName = tinyfd_saveFileDialog(
                filedialog,
                "mesh.json",
                1,
                l_SaveMeshfilterPattern,
                NULL);

        // error message if the file dialog was used wrong
        if (!lTheSaveFileName)
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