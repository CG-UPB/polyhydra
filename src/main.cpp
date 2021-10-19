
#include "Window.h"
#include "panels/LogWindow.h"
#include "panels/LogWindow.cpp"

int main() {

    // vOS::LogWindow my_log;
    // my_log.addLog("Hello %d world\n", 123);
    // my_log.draw("title");

    vOS::Window window(1280, 720, "volumeshOS");
    window.show();

    return 0;
}
