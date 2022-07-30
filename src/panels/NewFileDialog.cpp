
#include "NewFileDialog.h"

namespace volumeshOS::Internal
{
    char const* NewFileDialog::open_dialog(const char* filedialog)
    {

        // Variable for the filename
        char const* lTheOpenFileName;

        // opens the open file dialog - for more information see https://github.com/native-toolkit/tinyfiledialogs
        lTheOpenFileName = tinyfd_openFileDialog(
                filedialog,
                "",
                1,
                m_open_filter_pattern,
                nullptr,
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
            return nullptr;
        }
        return lTheOpenFileName;
    }

    char const* NewFileDialog::save_dialog(const char* filedialog, const std::string& default_name)
    {
        char const* lTheSaveFileName;
        // opens the save file dialog - for more information see https://github.com/native-toolkit/tinyfiledialogs
        lTheSaveFileName = tinyfd_saveFileDialog(
                filedialog,
                default_name.c_str(),
                2,
                m_save_filter_pattern,
                nullptr);

        // error message if the file dialog was used wrong
        if (!lTheSaveFileName)
        {
            tinyfd_messageBox(
                    "Information",
                    "No Filename received",
                    "ok",
                    "info",
                    1);
            return nullptr;
        }

        return lTheSaveFileName;
    }


    char const* NewFileDialog::load_mesh_settings(const char* filedialog)
    {

        // Variable for the filename
        char const* lTheOpenFileName;

        // opens the open file dialog - for more information see https://github.com/native-toolkit/tinyfiledialogs
        lTheOpenFileName = tinyfd_openFileDialog(
                filedialog,
                "",
                1,
                m_load_mesh_filter_pattern,
                nullptr,
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
            return nullptr;
        }
        return lTheOpenFileName;
    }

    char const* NewFileDialog::save_mesh_settings(const char* filedialog)
    {
        char const* lTheSaveFileName;
        // opens the save file dialog - for more information see https://github.com/native-toolkit/tinyfiledialogs
        lTheSaveFileName = tinyfd_saveFileDialog(
                filedialog,
                "mesh.json",
                1,
                m_save_mesh_filter_pattern,
                nullptr);

        // error message if the file dialog was used wrong
        if (!lTheSaveFileName)
        {
            tinyfd_messageBox(
                    "Information",
                    "No Filename received",
                    "ok",
                    "info",
                    1);
            return nullptr;
        }

        return lTheSaveFileName;
    }
}