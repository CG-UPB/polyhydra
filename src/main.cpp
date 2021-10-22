
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
    win->Open();

    delete win;

    return 0;
}
