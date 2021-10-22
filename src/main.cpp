
#include "Window.h"
#include "panels/LogWindow.h"
#include "panels/LogWindow.cpp"

int main() {

    vOS::Window window(1280, 720, "volumeshOS");
    window.show();

    return 0;
}
