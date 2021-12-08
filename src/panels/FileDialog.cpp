
#include "FileDialog.h"
#include <iostream>

#include "ImGuiFileDialog.h"

namespace vOS
{

    void FileDialog::show()
    {
        m_ok = false;
        if (ImGuiFileDialog::Instance()->Display("ChooseOVMFIle", ImGuiWindowFlags_NoCollapse, ImVec2(400,200), ImVec2(1200,600)))
        {
            // action if OK
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
                m_ok = true;
                m_file_path = std::string(filePathName);
            }
            ImGuiFileDialog::Instance()->Close();
        }
    }

    void FileDialog::open(const std::string& extension)
    {
        if (!m_is_open)
        {
            ImGuiFileDialog::Instance()->OpenDialog("ChooseOVMFIle", "Choose File", extension.c_str(), ".");
            m_is_open = true;
        }
    }

    bool FileDialog::is_ok() const
    {
        return m_ok;
    }

    const std::string& FileDialog::get_file_path()
    {
        return m_file_path;
    }

    void FileDialog::set_open(bool open)
    {
        m_is_open = open;
    }
}