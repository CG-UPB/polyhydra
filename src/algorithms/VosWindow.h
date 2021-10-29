//
// Created by jan on 15.10.21.
//

#ifndef VOLUMESHOS_VOSWINDOW_H
#define VOLUMESHOS_VOSWINDOW_H

#include <iostream>
#include <OpenVolumeMesh/Mesh/HexahedralMesh.hh>
#include <OpenVolumeMesh/Mesh/PolyhedralMesh.hh>
#include <thread>
#include <list>
#include <map>
#include <string>
#include "../Window.h"
#include "../panels/LogWindow.h"
#include "memory"

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


public:
    enum class Selection_Mode {ADD, SUBTRACT, TOGGLE, SET};
    enum class Translation_Mode {ADD, SET, MULTIPLY};
    enum class Rendering_Mode {DRAW, HIDE, HIGHLIGHT};

    // Renamed Classes for convenience
    using v3f = OpenVolumeMesh::GeometricPolyhedralMeshV3f;
    typedef void(*button_callback)(int button_id, bool flanked);
    typedef void(*parameter_callback)(int double_id, double value);

    typedef void(*vertex_selection_callback)(OpenVolumeMesh::VertexHandle* vertices_array, int length, Selection_Mode selection_mode);
    typedef void(*edge_selection_callback)(OpenVolumeMesh::EdgeHandle* vertices_array, int length, Selection_Mode selection_mode);
    typedef void(*face_selection_callback)(OpenVolumeMesh::FaceHandle* vertices_array, int length, Selection_Mode selection_mode);
    typedef void(*cell_selection_callback)(OpenVolumeMesh::CellHandle* vertices_array, int length, Selection_Mode selection_mode);

    typedef void(*operation_translation_callback)(double x, double y, double z, Translation_Mode translation_mode);
    typedef void(*operation_rendering_callback)(Rendering_Mode translation_mode);

    typedef void(*void_callback)();
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////// Initialization ////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /*
     * Opens a plain VolumeOs window. It will render any linked Meshes according to preset Preferences
     */
    VosWindow();
    /*
     * Opens a plain VolumeOs window. It will render any linked Meshes according to preset Preferences
     * Also links given mesh immediatly
     */
    VosWindow(OpenVolumeMesh::GeometricPolyhedralMeshV3f* mesh);
    /*
     * Deconstructor
     */
    ~VosWindow();


    LogWindow* Log(){return LogWindow::getInstance();}

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////// Meshes /////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////// Algorithm to Vos ////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    /*
     * Resets all given materials and color values back to the default value
     */
    void DefaultAll();

    /*
     * Applies the given material shader to all vertices in array
     * TODO: Only for Vertices at the moment
     */
    void ApplyMaterial(OpenVolumeMesh::VertexHandle* vertices_array, std::string material_path);

    /*
     * Applies given material shader to all elements with given property
     */
    void ApplyMaterial(std::string property, std::string material_path);

    /*
     * Draws all vertices in array in the given color
     * TODO: Only for Vertices at the moment
     */
    void SetColor(OpenVolumeMesh::VertexHandle* vertices_array, float r, float g, float b, float a);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////// Callback Interface ///////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////// Queueries ///////////////////////////////////////////////////////////////////////////////////////////////////
    /*
    * Asks whether Vos is ready for an algorithm side change to the linked meshes. Will be false if the user pressed the 'Pause' button, an internal
    * timer is not ready yet or some other underlying issue.
    */
    bool NextStepAllowed();

    /*
     * Asks whether Vos is paused
     */
    bool IsPaused();

    ///////////////////////////////////////////////////// Buttons ////////////////////////////////////////////////////////////////////////////////////////////////////

    /*
     * Sets Callback Function which is called when the user presses the Pause Button inside Vos
     */
    void SetCallbackPauseActivated(void_callback vc);

    /*
     * Sets Callback Function which is called when the user releases the Pause Button inside Vos
     */
    void SetCallbackPauseDeactivated(void_callback vc);
    /*
     * Sets Callback Function which is called when the user presses the Reset Button
     */
    void SetCallbackOnReset(void_callback vc);
    /*
     * Sets Callback Function which is called when the user presses the Step Button
     */
    void SetCallbackOnStep(void_callback vc);

    /////////////////////////////////////////////////// Selections //////////////////////////////////////////////////////////////////////////////////////////////////

    /*
     * Sets Callback Function which is called when the user performs a selection operation on vertices
     */
    void SetCallbackVertexSelection(vertex_selection_callback vsc){OnVerticesSelection = vsc;};
    /*
     * Sets Callback Function which is called when the user performs a selection operation on edges
     */
    void SetCallbackEdgeSelection(edge_selection_callback esc){OnEdgeSelection = esc;};
    /*
     * Sets Callback Function which is called when the user performs a selection operation on faces
     */
    void SetCallbackFaceSelection(face_selection_callback fsc){OnFaceSelection = fsc;};
    /*
     * Sets Callback Function which is called when the user performs a selection operation on cells
     */
    void SetCallbackCellSelection(cell_selection_callback csc) {OnCellSelection = csc;};

    bool is_running();

private:

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////// Variables ////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Threading Variables
    static int static_thread_id;
    int thread_id = 0;

    // Variables
    v3f* m_mesh_reference;
    Window* m_window;
    bool m_initialized= false;
    bool m_running;

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////// Main Loop /////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::thread* main_loop_thread;

    // Main function that will be run parallel in a thread
    void Main_Loop();

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////// Callback Interface ///////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////// Callback Buttons ////////////////////////////////////////////////////////////////////////////////////////////////
    /*
     * Called when the User Pressed or Released the Pause Button
     */
    void PauseButtonFlank(bool is_pressed){if(is_pressed) OnPauseActivated(); else OnPauseDeactivated();};

    /////////////////////////////////////////////// Callback Functions ///////////////////////////////////////////////////////////////////////////////////////////////
    static void default_callback_function() {std::cout << "Debug: Default Callback Function Called" << std::endl;};

    static void default_vertex_selection_function(OpenVolumeMesh::VertexHandle* vertices_array, int length, Selection_Mode selection_mode){};
    static void default_edge_selection_function(OpenVolumeMesh::EdgeHandle* edge_array, int length, Selection_Mode selection_mode){};
    static void default_face_selection_function(OpenVolumeMesh::FaceHandle* face_array, int length, Selection_Mode selection_mode){};
    static void default_cell_selection_function(OpenVolumeMesh::CellHandle* cell_array, int length, Selection_Mode selection_mode){};

    static void default_translate_operation_function(double x, double y, double z, Translation_Mode translation_mode){};
    static void default_rendering_operation_function(Rendering_Mode rendering_mode){};

    /// Call Back Functions
    /// User Input Reactions
    // Called when Pause Button has been pressed and is active
    void_callback OnPauseActivated = default_callback_function;
    // Called when Pause Button has been released and is inactive
    void_callback OnPauseDeactivated = default_callback_function;
    // Called when Reset Button has been pressed
    void_callback OnResetPressed = default_callback_function;
    // Called when Step Button has been pressed
    void_callback OnStepPressed = default_callback_function;
    // Called when internal Step Timer has reached < 0
    void_callback OnStepTimerLap  = default_callback_function;
    // Called when a number of vertices have been selected
    vertex_selection_callback OnVerticesSelection = default_vertex_selection_function;
    // Called when a number of edges have been selected
    edge_selection_callback OnEdgeSelection = default_edge_selection_function;
    // Called when a number of faces have been selected
    face_selection_callback OnFaceSelection = default_face_selection_function;
    // Called when a number of cells have been selected
    cell_selection_callback OnCellSelection = default_cell_selection_function;

    operation_translation_callback OnTranslateOperation = default_translate_operation_function;
    operation_rendering_callback OnRenderingOperation = default_rendering_operation_function;
    // Generally Called when the User does anything to the Mesh (debug)
    void_callback OnUpdate = default_callback_function;


};

}


#endif //VOLUMESHOS_VOSWINDOW_H
