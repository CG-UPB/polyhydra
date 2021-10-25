
#include "algorithms/VosWindow.h"
vOS::VosWindow* window = nullptr;

void pauseCallbackTest(){
    if(window != nullptr) window->Log()->addLog("Pause");
}

void unpauseCallbackTest(){
    if(window != nullptr) window->Log()->addLog("Unpause");
}

int main() {
    window = new vOS::VosWindow();

    for(int i = 0; i < 1000; i++)
        window->Log()->addLog(std::to_string(i));

    window->SetCallbackPauseActivated(pauseCallbackTest);
    window->SetCallbackPauseDeactivated(unpauseCallbackTest);

    delete window;

    return 0;
}
