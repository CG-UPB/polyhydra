
#include "algorithms/VosWindow.h"
#include "panels/LogWindow.h"
#include "mesh/mesh_object.h"
vOS::VosWindow* window = nullptr;
#include "mesh/mesh_object.h"

void pauseCallbackTest(){
    if(window != nullptr) window->Log()->addLog("Pause");
}

void unpauseCallbackTest(){
    if(window != nullptr) window->Log()->addLog("Unpause");
}

int main() {
    window = new vOS::VosWindow();

    OpenVolumeMesh::GeometricPolyhedralMeshV3f* v3f;

    window->LinkMesh(v3f);

    window->SetCallbackPauseActivated(pauseCallbackTest);
    window->SetCallbackPauseDeactivated(unpauseCallbackTest);

    delete window;

    return 0;
}
