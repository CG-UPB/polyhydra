
#include "Window.h"
#include "algorithms/VosWindow.h"

void pauseCallbackTest(){
    std::cout << "Pause" << std::endl;
}

void unpauseCallbackTest(){
    std::cout << "Unpause" << std::endl;
}

int main() {

    vOS::VosWindow* win = new vOS::VosWindow();
    win->SetCallbackPauseDeactivated(unpauseCallbackTest);
    win->SetCallbackPauseActivated(pauseCallbackTest);
    win->PauseButtonFlank(true);
    win->PauseButtonFlank(false);
    win->PauseButtonFlank(true);
    win->PauseButtonFlank(false);

    vOS::Window window(1280, 720, "volumeshOS");
    window.show();

    return 0;
}
