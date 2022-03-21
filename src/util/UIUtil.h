#pragma once

#include <string>
#include <unordered_map>
#include "../rendering/gl/TextureIcon.h"
#include "imgui.h"

namespace vOS
{
    class UIUtil
    {
    public:

        static void load_all();
        static void delete_all();

        static TextureIcon* get_icon(const std::string& name);

        static ImFont* get_regular_font();
        static ImFont* get_bold_font();

    private:

        static ImFont* s_regular;
        static ImFont* s_bold;

        static std::unordered_map<std::string, TextureIcon*> s_icons;
    };
}