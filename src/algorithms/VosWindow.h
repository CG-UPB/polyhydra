//
// Created by jan on 15.10.21.
//

#ifndef VOLUMESHOS_VOSWINDOW_H
#define VOLUMESHOS_VOSWINDOW_H

#include <iostream>
#include <OpenVolumeMesh/Mesh/HexahedralMesh.hh>
#include <OpenVolumeMesh/Mesh/PolyhedralMesh.hh>

namespace vOS
{
/*
 * Used by the programmer to visualize OVM Meshes using volumeshos
 * Can be used to highlight specific elements, color specific parts of the mesh and apply other configerations
 * Provides Callback functions which communicate user inputs back to the algorithm
 *
 * TODO: Uses GeomtericPolyhedralMeshV3f exlusively at the moment, waiting for Wrapper Class
 * TODO: Allows a single Mesh Visualization at the moment
 */
class VosWindow {

    // Renamed Classes for convenience
    using v3f = OpenVolumeMesh::GeometricPolyhedralMeshV3f;
    typedef void(*void_callback)();
public:
    /*
     * Constructor
     */
    VosWindow();
    /*
     * Deconstructor
     */
    ~VosWindow();

    /*
     * Opens a plain VolumeOs window. It will render any linked Meshes according to preset Preferences
     * Returns true on success, otherwise false (see Log for specific info)
     */
    bool OpenWindow();

    /*
     * Links a single (v3f) OVM Mesh to be rendered by the Vos Window
     * Returns true on success, otherwise false (see Log for specific info)
     */
    bool LinkMesh(v3f* mesh);
    /*
     * Removes an already existing (v3f) OVM Mesh from the Vos Window. It will no longer be rendered or interacted with
     * Returns true on success, otherwise false  (see Log for specific info)
     */
    bool RemoveMesH(v3f* mesh);

    /*
     * Sets Callback Function which is called when the user presses the Pause Button inside Vos
     */
    void SetCallbackPauseActivated(void_callback vc);

    /*
     * Sets Callback Function which is called when the user releases the Pause Button inside Vos
     */
    void SetCallbackPauseDeactivated(void_callback vc);

    /*
     * Asks whether Vos is ready for an algorithm side change to the linked meshes. Will be false if the user pressed the 'Pause' button, an internal
     * timer is not ready yet or some other underlying issue.
     */
    bool VosSideReady();

    /*
     * Asks whether Vos is paused
     */
    bool VosPauseActive();


    /*
     * Called when the User Pressed or Released the Pause Button
     */
    void PauseButtonFlank(bool is_pressed){if(is_pressed) OnPauseActivated(); else OnPauseDeactivated();};

private:
    v3f* m_mesh;

    // Callback functions
    static void default_callback_function() {std::cout << "Debug: Default Callback Function Called" << std::endl;};

    void_callback OnPauseActivated = default_callback_function;
    void_callback OnPauseDeactivated = default_callback_function;
};

}


#endif //VOLUMESHOS_VOSWINDOW_H
