#include "Window.h"

int main() {

    vOS::Window window(1280, 720, "volumeshOS");
    window.show();
    window.destroy();

    return 0;
}
