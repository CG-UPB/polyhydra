#pragma once

#include "../ImguiRenderer.h"

namespace vOS
{
    class FileDialog : public WindowPanel
    {
    public:
        ~FileDialog() override = default;
        void show() override;

        void open(const std::string& extension);
        const std::string& get_file_path();
        void set_open(bool open);

        bool is_ok() const;

    private:

        bool m_is_open = false;
        bool m_ok = false;
        std::string m_file_path;
    };
}