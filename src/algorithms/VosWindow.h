#pragma once
//
// Created by jan on 15.10.21.
//

#ifndef VOLUMESHOS_VOSWINDOW_H
#define VOLUMESHOS_VOSWINDOW_H

#include <iostream>
#include <OpenVolumeMesh/Mesh/HexahedralMesh.hh>
#include <OpenVolumeMesh/Mesh/PolyhedralMesh.hh>
#include <thread>
#include <string>
#include <functional>
#include "../Window.h"
#include "../panels/LogWindow.h"
#include "../mesh/MeshObject.h"


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
    class VosWindow
    {


    public:
        enum class Selection_Mode
        {
            ADD, SUBTRACT, TOGGLE, SET
        };
        enum class Translation_Mode
        {
            ADD, SET, MULTIPLY
        };
        enum class Rendering_Mode
        {
            DRAW, HIDE, HIGHLIGHT
        };

        static VosWindow& instance();

        // Renamed Classes for convenience
        using v3f = OpenVolumeMesh::GeometricPolyhedralMeshV3f;

    typedef std::function<void(OpenVolumeMesh::VertexHandle* vertices_array, int length, Selection_Mode selection_mode)> vertex_selection_callback;
    typedef std::function<void(OpenVolumeMesh::EdgeHandle* edges_array, int length, Selection_Mode selection_mode)> edge_selection_callback;
    typedef std::function<void(OpenVolumeMesh::FaceHandle* faces_array, int length, Selection_Mode selection_mode)> face_selection_callback;
    typedef std::function<void(OpenVolumeMesh::CellHandle* cells_array, int length, Selection_Mode selection_mode)> cell_selection_callback;

    typedef std::function<void(double x, double y, double z, Translation_Mode translation_mode)> operation_translation_callback;
    typedef std::function<void(Rendering_Mode rendering_mode)> operation_rendering_callback;

    typedef std::function<void()> void_callback;
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////// Logger ////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        /*
         * Returns vos window log
         */
        LogWindow* Log(){return LogWindow::getInstance();}
        /*
         * Adds the given string to the vos window log. Shortcut function, so that Log() does not have to be called
         */
        void add_log(const char* fmt, int level = 0);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////// Meshes /////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void set_vertex_color(OpenVolumeMesh::VertexHandle *vertices_array, float r, float g, float b, float a);

        void set_mesh(OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3f> *mesh);

        MeshObject& get_mesh_obj();
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
        void ApplyMaterial(OpenVolumeMesh::VertexHandle *vertices_array, std::string material_path);

        /*
         * Applies given material shader to all elements with given property
         */
        void ApplyMaterial(std::string property, std::string material_path);


        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////// Callback Interface ///////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////// Queueries ///////////////////////////////////////////////////////////////////////////////////////////////////
    /*
    * Asks whether Vos is ready for an algorithm side change to the linked meshes. Will be false if the user pressed the 'Pause' button,
     * the window or mesh is missing or hasn't been fully initialized yet or some other underlying issue is present.
    */
     bool is_ready();

    /*
     * Asks whether Vos is paused
     */
     bool is_paused();

        ///////////////////////////////////////////////////// Buttons ////////////////////////////////////////////////////////////////////////////////////////////////////

    /*
     * Sets Callback Function which is called when the user presses the Pause Button inside Vos
     * Remember using std::bind(function, this) to the callback function parameter
     */
    void set_callback_paused(void_callback vc);

    /*
     * Sets Callback Function which is called when the user releases the Pause Button inside Vos
     * Remember using std::bind(function, this) to the callback function parameter
     */
    void set_callback_unpaused(void_callback vc) ;
    /*
     * Sets Callback Function which is called when the user presses the Reset Button
     * Remember using std::bind(function, this) to the callback function parameter
     */
    void set_callback_reset(void_callback vc) { m_on_reset = vc;};
    /*
     * Sets Callback Function which is called when the user presses the Step Button
     * Remember using std::bind(function, this) to the callback function parameter
     */
    void set_callback_step(void_callback vc){ m_on_step = vc;};

        /////////////////////////////////////////////////// Selections //////////////////////////////////////////////////////////////////////////////////////////////////

    /*
     * Sets Callback Function which is called when the user performs a selection operation on vertices
     */
    void SetCallbackVertexSelection(vertex_selection_callback vsc){m_on_vertex_selection = vsc;};
    /*
     * Sets Callback Function which is called when the user performs a selection operation on edges
     */
    void SetCallbackEdgeSelection(edge_selection_callback esc){m_on_edge_selection = esc;};
    /*
     * Sets Callback Function which is called when the user performs a selection operation on faces
     */
    void SetCallbackFaceSelection(face_selection_callback fsc){m_on_face_selection = fsc;};
    /*
     * Sets Callback Function which is called when the user performs a selection operation on cells
     */
    void SetCallbackCellSelection(cell_selection_callback csc) {m_on_cell_selection = csc;};

        static bool is_running();

        void main_loop();

    private:
    /// User Input Reactions
    // Called when Pause Button has been pressed and is active
    void_callback m_on_vos_paused = default_callback_function;
    // Called when Pause Button has been released and is inactive
    void_callback m_on_vos_unpaused = default_callback_function;
    // Called when Reset Button has been pressed
    void_callback m_on_reset = default_callback_function;
    // Called when Step Button has been pressed
    void_callback m_on_step = default_callback_function;
    // Called when a number of vertices have been selected
    vertex_selection_callback m_on_vertex_selection = default_vertex_selection_function;
    // Called when a number of edges have been selected
    edge_selection_callback m_on_edge_selection = default_edge_selection_function;
    // Called when a number of faces have been selected
    face_selection_callback m_on_face_selection = default_face_selection_function;
    // Called when a number of cells have been selected
    cell_selection_callback m_on_cell_selection = default_cell_selection_function;

    operation_translation_callback m_on_translate_operation = default_translate_operation_function;
    operation_rendering_callback m_on_rendering_operation = default_rendering_operation_function;
    // Generally Called when the User does anything to the Mesh (debug)
    void_callback m_on_general_update = default_callback_function;

        VosWindow();
        ~VosWindow();

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////// Variables ////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Threading Variables
        static int static_thread_id;
        int thread_id = 0;

        // Variables
        v3f *m_mesh_reference;
        Window *m_window;
        MeshObject m_mesh_obj;
        bool m_initialized = false;
        bool m_running;
        bool m_window_paused = false;

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////// Main Loop /////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::thread *main_loop_thread;

        // Main function that will be run parallel in a thread


        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////// Callback Interface ///////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////// Callback Functions ///////////////////////////////////////////////////////////////////////////////////////////////

    /// Default Empty Callback Function
    static void default_callback_function();

    static void default_vertex_selection_function(OpenVolumeMesh::VertexHandle* vertices_array, int length, Selection_Mode selection_mode){};
    static void default_edge_selection_function(OpenVolumeMesh::EdgeHandle* edge_array, int length, Selection_Mode selection_mode){};
    static void default_face_selection_function(OpenVolumeMesh::FaceHandle* face_array, int length, Selection_Mode selection_mode){};
    static void default_cell_selection_function(OpenVolumeMesh::CellHandle* cell_array, int length, Selection_Mode selection_mode){};

    static void default_translate_operation_function(double x, double y, double z, Translation_Mode translation_mode){};
    static void default_rendering_operation_function(Rendering_Mode rendering_mode){};

    /// Call Back Functions

    };

}


#endif //VOLUMESHOS_VOSWINDOW_H
