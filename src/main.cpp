
#include "algorithms/VosWindow.h"
#include "algorithms/Dijkstra.h"
vOS::VosWindow* window = nullptr;

void pauseCallbackTest()
{
    std::cout << "Pause" << std::endl;
}

void unpauseCallbackTest()
{
    std::cout << "Unpause" << std::endl;
}

int main()
{
    vOS::Dijkstra dijkstra;
    dijkstra.run();

    vOS::VosWindow* window = new vOS::VosWindow();

    for(int i = 0; i < 1000; i++)
        window->Log()->addLog(std::to_string(i));

    window->SetCallbackPauseActivated(pauseCallbackTest);
    window->SetCallbackPauseDeactivated(unpauseCallbackTest);

    delete window;

    return 0;
}
