
#include "algorithms/VosWindow.h"
#include "algorithms/Dijkstra.h"
vOS::VosWindow* window = nullptr;

void pauseCallbackTest(){
    if(window != nullptr) window->Log()->addLog("Pause");
}

void unpauseCallbackTest(){
    if(window != nullptr) window->Log()->addLog("Unpause");
}

int main() {
    vOS::Dijkstra dijkstra;
    dijkstra.run();

    window = new vOS::VosWindow();

    for(int i = 0; i < 1000; i++)
        window->Log()->addLog(std::to_string(i));

    window->SetCallbackPauseActivated(pauseCallbackTest);
    window->SetCallbackPauseDeactivated(unpauseCallbackTest);

    delete window;

    return 0;
}
