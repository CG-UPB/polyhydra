#pragma once

#include "polyhydra/Window.h"

#include "tinyfiledialogs.h"

namespace polyhydra::Internal
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
     *
     * This method creates a file dialog, which returns the path of a ovm-file, after the user has chosen some. If
     * the user cancels the Input, or gives a wrong file ending an error-prompt will be displayed and return value will
     * be "NULL"
     *
     * @param filedialog - the title of the window prompt
     * @return char const * filename, or NULL
     */
    char const* open_dialog(const char* filedialog);

    /**
     *
     * This method creates a file dialog, which returns the path of a png- or bmp-file, after the user has chosen some.
     * If the user cancels the Input, or gives a wrong file ending an error-prompt will be displayed and return value
     * will be "NULL"
     *
     * @param filedialog - the title of the window prompt
     * @param default_name - the default name of the file
     * @return char const * filename, or NULL
     */
    char const* save_dialog(const char* filedialog, const std::string& default_name = "file.png");

    /**
     *
     * This method creates a file dialog, which returns the path of a json file, after the user has chosen some. If
     * the user cancels the Input, or gives a wrong file ending an error-prompt will be displayed and return value will
     * be "NULL"
     *
     * @param filedialog - the title of the window prompt
     * @return char const * filename, or NULL
     */
    char const* load_mesh_settings(const char* filedialog);

    /**
     *
     * This method creates a file dialog, which returns the path of a json-file, after the user has chosen some. If
     * the user cancels the Input, or gives a wrong file ending an error-prompt will be displayed and return value will
     * be "NULL"
     *
     * @param filedialog - the title of the window prompt
     * @return char const * filename, or NULL
     */
    char const* save_mesh_settings(const char* filedialog);

  private:
    // file ending for the openfile-dialog
    const char* m_open_filter_pattern[1] = {"*.ovm"};
    // file endings for the savefile-dialog
    const char* m_save_filter_pattern[2] = {"*.png", "*.bmp"};

    // file ending for the update-mesh-data-dialog
    const char* m_load_mesh_filter_pattern[1] = {"*.json"};
    // file ending for the save-mesh-data-dialog
    const char* m_save_mesh_filter_pattern[1] = {"*.json"};
};
} // namespace polyhydra::Internal
