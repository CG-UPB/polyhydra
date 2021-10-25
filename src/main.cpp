
#include "algorithms/VosWindow.h"

void pauseCallbackTest(){
    std::cout << "Pause" << std::endl;
}

void unpauseCallbackTest(){
    std::cout << "Unpause" << std::endl;
}

int main() {
    vOS::VosWindow* window = new vOS::VosWindow();

    window->Open();

    delete window;

    return 0;
}
