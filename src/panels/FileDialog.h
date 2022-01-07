#pragma once

#include "../ImguiRenderer.h"
#include "ImGuiFileDialog.h"

namespace vOS
{
    class FileDialog : public WindowPanel
    {
    public:
        ~FileDialog() override = default;
        void show() override;

        void close();

        void open(const std::string& extension, int nbr_of_dialog);
        const std::string& get_file_path_file_loader();
        const std::string& get_file_path_snapshot_saver();
        void set_open_fileloader(bool open);
        void set_open_snapshot_saver(bool open);
        bool file_dialogue_open(){return m_is_open_fileloader || m_is_open_snapshot_saver;}
        bool is_ok_file_loader() const;
        bool is_ok_snapshot_saver() const;

    private:

        bool m_open_file = true;
        bool m_open_snap = true;
        bool m_is_open_fileloader = false;
        bool m_is_open_snapshot_saver = false;
        bool m_ok_file_loader = false;
        bool m_ok_snapshot_saver = false;
        std::string m_file_path_file_loader;
        std::string m_file_path_snapshot_saver;
        ImGuiFileDialog m_instance_file_loader;
        ImGuiFileDialog m_instance_snapshot_saver;
    };
}