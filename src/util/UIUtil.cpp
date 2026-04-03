
#include "polyhydra/util/UIUtil.h"

#include "polyhydra/fs/FileManager.h"

namespace polyhydra::Internal
{
std::unordered_map<std::string, std::shared_ptr<TextureIcon>> UIUtil::s_icons;
ImFont* UIUtil::s_regular = nullptr;
ImFont* UIUtil::s_bold = nullptr;

void UIUtil::load_all()
{
    FS_NAMESPACE::path icon_path = FileManager::get_resource_path() / "icons";
    for (auto& file : FS_NAMESPACE::recursive_directory_iterator(icon_path))
    {
        if (FS_NAMESPACE::is_directory(file))
        {
            continue;
        }
        std::string file_name = file.path().filename().string();
        auto icon = std::make_shared<TextureIcon>(file.path());
        s_icons[file_name] = icon;
    }
    FS_NAMESPACE::path font_path = FileManager::get_resource_path() / "fonts";
    FS_NAMESPACE::path regular_path = font_path / "Roboto-Regular.ttf";
    FS_NAMESPACE::path bold_path = font_path / "Roboto-Bold.ttf";
    s_regular = ImGui::GetIO().Fonts->AddFontFromFileTTF(regular_path.string().c_str(), 17.0f);
    s_bold = ImGui::GetIO().Fonts->AddFontFromFileTTF(bold_path.string().c_str(), 17.0f);
    ImGui::GetIO().FontDefault = s_regular;
}

void UIUtil::clean_up()
{
    for (auto& icon : s_icons)
    {
        icon.second.reset();
    }
}

std::shared_ptr<TextureIcon> UIUtil::get_icon(const std::string& name)
{
    auto icon = s_icons.find(name);
    if (icon == s_icons.end())
    {
        throw std::invalid_argument("Could not find icon: " + name);
    }
    return s_icons[name];
}

ImFont* UIUtil::get_regular_font()
{
    return s_regular;
}

ImFont* UIUtil::get_bold_font()
{
    return s_bold;
}
} // namespace polyhydra::Internal