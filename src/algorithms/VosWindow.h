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
#include <list>
#include <map>
#include <string>
#include "../Window.h"
#include "../panels/LogWindow.h"
#include "../mesh/MeshObject.h"
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

        typedef void(*button_callback)(int button_id, bool flanked);

        typedef void(*parameter_callback)(int double_id, double value);

        typedef void(*vertex_selection_callback)(OpenVolumeMesh::VertexHandle *vertices_array, int length,
                                                 Selection_Mode selection_mode);

        typedef void(*edge_selection_callback)(OpenVolumeMesh::EdgeHandle *vertices_array, int length,
                                               Selection_Mode selection_mode);

        typedef void(*face_selection_callback)(OpenVolumeMesh::FaceHandle *vertices_array, int length,
                                               Selection_Mode selection_mode);

        typedef void(*cell_selection_callback)(OpenVolumeMesh::CellHandle *vertices_array, int length,
                                               Selection_Mode selection_mode);

        typedef void(*operation_translation_callback)(double x, double y, double z, Translation_Mode translation_mode);

        typedef void(*operation_rendering_callback)(Rendering_Mode translation_mode);

        typedef void(*void_callback)();
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////// Initialization ////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        LogWindow *Log(){ return LogWindow::getInstance(); }

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
        * Asks whether Vos is ready for an algorithm side change to the linked meshes. Will be false if the user pressed the 'Pause' button, an internal
        * timer is not ready yet or some other underlying issue.
        */
        static bool next_step_allowed();

        /*
         * Asks whether Vos is paused
         */
        static bool is_paused();

        ///////////////////////////////////////////////////// Buttons ////////////////////////////////////////////////////////////////////////////////////////////////////

        /*
         * Sets Callback Function which is called when the user presses the Pause Button inside Vos
         */
        void set_callback_pause_activated(void_callback vc);

        /*
         * Sets Callback Function which is called when the user releases the Pause Button inside Vos
         */
        void set_callback_pauseDeactivated(void_callback vc);

        /*
         * Sets Callback Function which is called when the user presses the Reset Button
         */
        void set_callback_on_reset(void_callback vc);

        /*
         * Sets Callback Function which is called when the user presses the Step Button
         */
        void set_callback_on_step(void_callback vc);

        /////////////////////////////////////////////////// Selections //////////////////////////////////////////////////////////////////////////////////////////////////

        /*
         * Sets Callback Function which is called when the user performs a selection operation on vertices
         */
        void set_callback_vertex_selection(vertex_selection_callback vsc){ OnVerticesSelection = vsc; };


        /*
         * Sets Callback Function which is called when the user performs a selection operation on edges
         */
        void set_callback_edge_selection(edge_selection_callback esc){ OnEdgeSelection = esc; };

        /*
         * Sets Callback Function which is called when the user performs a selection operation on faces
         */
        void set_callback_face_selection(face_selection_callback fsc){ OnFaceSelection = fsc; };

        /*
         * Sets Callback Function which is called when the user performs a selection operation on cells
         */
        void set_callback_cell_selection(cell_selection_callback csc){ OnCellSelection = csc; };

        static bool is_running();

        void main_loop();

    private:

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

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////// Main Loop /////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::thread *main_loop_thread;

        // Main function that will be run parallel in a thread


        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////// Callback Interface ///////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        //////////////////////////////////////////////// Callback Buttons ////////////////////////////////////////////////////////////////////////////////////////////////
        /*
         * Called when the User Pressed or Released the Pause Button
         */
        void PauseButtonFlank(bool is_pressed)
        { if (is_pressed) on_pause_activated(); else OnPauseDeactivated(); };

        /////////////////////////////////////////////// Callback Functions ///////////////////////////////////////////////////////////////////////////////////////////////
        static void default_callback_function()
        {
            std::cout << "Debug: Default Callback Function Called" << std::endl;
        };

        static void default_vertex_selection_function(OpenVolumeMesh::VertexHandle *vertices_array, int length,
                                                      Selection_Mode selection_mode)
        {};

        static void default_edge_selection_function(OpenVolumeMesh::EdgeHandle *edge_array, int length,
                                                    Selection_Mode selection_mode)
        {};

        static void default_face_selection_function(OpenVolumeMesh::FaceHandle *face_array, int length,
                                                    Selection_Mode selection_mode)
        {};

        static void default_cell_selection_function(OpenVolumeMesh::CellHandle *cell_array, int length,
                                                    Selection_Mode selection_mode)
        {};

        static void
        default_translate_operation_function(double x, double y, double z, Translation_Mode translation_mode)
        {};

        static void default_rendering_operation_function(Rendering_Mode rendering_mode)
        {};

        /// Call Back Functions
        /// User Input Reactions
        // Called when Pause Button has been pressed and is active
        void_callback on_pause_activated = default_callback_function;
        // Called when Pause Button has been released and is inactive
        void_callback OnPauseDeactivated = default_callback_function;
        // Called when Reset Button has been pressed
        void_callback on_reset_pressed = default_callback_function;
        // Called when Step Button has been pressed
        void_callback on_step_pressed = default_callback_function;
        // Called when internal Step Timer has reached < 0
        void_callback OnStepTimerLap = default_callback_function;
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
