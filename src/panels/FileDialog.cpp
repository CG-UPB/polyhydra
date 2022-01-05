
#include "FileDialog.h"
#include "LogWindow.h"

#include <iomanip>
#include <ctime>
#include <string>
#include <sstream>


namespace vOS
{

    void FileDialog::show()
    {

        if (m_instance_file_loader.Display("ChooseOVMFIle", ImGuiWindowFlags_NoCollapse, ImVec2(400,200), ImVec2(1200,600)))
        {
            if (m_open_file){
                m_ok_file_loader = false;
                m_open_file = false;
            }
            // action if OK
            if (m_instance_file_loader.IsOk())
            {
                std::string filePathName = m_instance_file_loader.GetFilePathName();
                std::string filePath = m_instance_file_loader.GetCurrentPath();
                m_ok_file_loader = true;
                m_open_file = true;
                m_file_path_file_loader = std::string(filePathName);
            }
            m_instance_file_loader.Close();
        }

        if (m_instance_snapshot_saver.Display("ChooseBMPFile", ImGuiWindowFlags_NoCollapse, ImVec2(400,200), ImVec2(1200,600)))
        {
            if (m_open_snap){
                m_ok_snapshot_saver = false;
                m_open_snap = false;
            }

            // action if OK
            LogWindow::getInstance()->addLog("Vor OK");
            if (m_instance_snapshot_saver.IsOk())
            {
                LogWindow::getInstance()->addLog("Nach OK");
                std::string filePathName = m_instance_snapshot_saver.GetFilePathName();
                std::string filePath = m_instance_snapshot_saver.GetCurrentPath();
                m_open_snap = true;
                m_ok_snapshot_saver = true;
                m_file_path_snapshot_saver = std::string(filePathName);;
            }
            LogWindow::getInstance()->addLog("Vor Close");
            m_instance_snapshot_saver.Close();
        }
    }

    void FileDialog::open(const std::string& extension, int nbr_of_dialog)
    {
        if (!m_is_open_fileloader && nbr_of_dialog == 0)
        {
            m_instance_file_loader.OpenDialog("ChooseOVMFIle", "Choose File", extension.c_str(), ".");
            m_is_open_fileloader = true;
        }
        if (!m_is_open_snapshot_saver && nbr_of_dialog == 1)
        {
            std::stringstream str;
            time_t rawtime;
            time ( &rawtime ); 
            str << rawtime;
            m_instance_snapshot_saver.OpenDialog("ChooseBMPFile", "Save File", extension.c_str(), ".",str.str());
            m_is_open_snapshot_saver = true;
        }
    }


    bool FileDialog::is_ok_file_loader() const
    {
        return m_ok_file_loader;
    }

    bool FileDialog::is_ok_snapshot_saver() const
    {
        return m_ok_snapshot_saver;
    }

    const std::string& FileDialog::get_file_path_file_loader()
    {
        return m_file_path_file_loader;
    }

    void FileDialog::set_open_fileloader(bool open)
    {
        m_is_open_fileloader = open;
    }

    const std::string& FileDialog::get_file_path_snapshot_saver()
    {
        return m_file_path_snapshot_saver;
    }

    void FileDialog::set_open_snapshot_saver(bool open)
    {
        m_is_open_snapshot_saver = open;
    }
}