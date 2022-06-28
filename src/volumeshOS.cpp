
#include "volumeshOS.h"

#include "rendering/Renderer.h"

namespace volumeshOS
{
    std::vector<std::function<void()>> commands;

    void set_color(const Color& color)
    {
        commands.emplace_back([&color](){
            // do stuff
        });
    }
}