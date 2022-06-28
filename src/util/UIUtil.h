#pragma once

#include "vospch.h"

#include "../rendering/gl/TextureIcon.h"

namespace volumeshOS::Internal
{
    class UIUtil
    {
    public:

        static void load_all();

        static void clean_up();

        static std::shared_ptr<TextureIcon> get_icon(const std::string& name);

        static ImFont* get_regular_font();
        static ImFont* get_bold_font();

    private:

        static ImFont* s_regular;
        static ImFont* s_bold;

        static std::unordered_map<std::string, std::shared_ptr<TextureIcon>> s_icons;
    };
}