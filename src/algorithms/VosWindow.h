#pragma once
//
// Created by jan on 15.10.21.
//

#ifndef VOLUMESHOS_VOSWINDOW_H
#define VOLUMESHOS_VOSWINDOW_H

#include <iostream>
#include <OpenVolumeMesh/Mesh/HexahedralMesh.hh>
#include <OpenVolumeMesh/Mesh/PolyhedralMesh.hh>
#include <string>
#include <functional>
#include "../ImguiRenderer.h"
#include "../mesh/MeshObject.h"
#include "../panels/CustomUIPanel.h"
#include "../panels/LogWindow.h"


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
        ///////////////////////////////////////////////////// Meshes /////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void set_vertex_color(OpenVolumeMesh::VertexHandle v_h, bool b, float red, float green, float blue, float alpha);

        void set_mesh(OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3f> *mesh);

        MeshObject& get_mesh_obj();
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////// Algorithm to Vos ////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


        /*
         * Resets all given materials and color values back to the default value
         */
        void default_all(){};

        /*
         * Applies the given material shader to all vertices in array
         * TODO: Only for Vertices at the moment
         */
        void apply_material(OpenVolumeMesh::VertexHandle *vertices_array, std::string material_path){};

        /*
         * Applies given material shader to all elements with given property
         */
        void apply_material(std::string property, std::string material_path){};


        void debugging_template_ui();

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////// Callback Interface ///////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::string get_loaded_file_name() {return m_loaded_file_path;};
        void set_loaded_file_path_name(std::string path) { m_loaded_file_path = path;};

        void set_custom_imgui(void_callback vc) { m_custom_ui->set_custom_callback((vc)); };

        // Queueries //////////////////////////////////////////////////////////////////////////////////////////////////
        /*
        * Asks whether Vos is ready for an algorithm side change to the linked meshes. Will be false if the user pressed the 'Pause' button,
         * the window or mesh is missing or hasn't been fully initialized yet or some other underlying issue is present.
        */
         bool is_ready();

         /*
          * Asks whether the Vos window has been closed by the user or not
          */
         bool is_closed();

        //Buttons //////////////////////////////////////////////////////////////////////////////////////////////////

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

        // Selections //////////////////////////////////////////////////////////////////////////////////////////////////

        /*
         * Sets Callback Function which is called when the user performs a selection operation on vertices
         */
        void set_callback_vertex_selection(vertex_selection_callback vsc){ m_on_vertex_selection = vsc;};
        /*
         * Sets Callback Function which is called when the user performs a selection operation on edges
         */
        void set_callback_edge_selection(edge_selection_callback esc){ m_on_edge_selection = esc;};
        /*
         * Sets Callback Function which is called when the user performs a selection operation on faces
         */
        void set_callback_face_selection(face_selection_callback fsc){ m_on_face_selection = fsc;};
        /*
         * Sets Callback Function which is called when the user performs a selection operation on cells
         */
        void set_callback_cell_selection(cell_selection_callback csc) { m_on_cell_selection = csc;};

        bool is_running();

        void open();

        void render_loop();
        bool render_manual();

        void close();

        // User Input Reactions //////////////////////////////////////////////////////////////////////////////////////////////////
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

    private:
        VosWindow();
        ~VosWindow();

        // Debugging //////////////////////////////////////////////////////////////////////////////////////////////////
        bool m_pause_toggled = false;

        // Variables //////////////////////////////////////////////////////////////////////////////////////////////////

        std::string m_loaded_file_path = "";

        bool m_initialized = false;
        bool m_window_open;
        bool m_window_paused = false;

        // References //////////////////////////////////////////////////////////////////////////////////////////////////

        CustomUIPanel* m_custom_ui;

        v3f *m_mesh_reference;
        ImguiRenderer *m_imgui_renderer;
        MeshObject m_mesh_obj;

        MenuBar* m_menu_bar;
        std::vector<WindowPanel*> m_panels;

        MenuBar* get_menu_bar(){return m_menu_bar;}

        // Main Loop //
        void initPanels();

        void render();

        // Callback Interface //////////////////////////////////////////////////////////////////////////////////////////////////

        // Callback Functions //////////////////////////////////////////////////////////////////////////////////////////////////
        /// Default Empty Callback Function
        static void default_callback_function();

        static void default_vertex_selection_function(OpenVolumeMesh::VertexHandle* vertices_array, int length, Selection_Mode selection_mode){};
        static void default_edge_selection_function(OpenVolumeMesh::EdgeHandle* edge_array, int length, Selection_Mode selection_mode){};
        static void default_face_selection_function(OpenVolumeMesh::FaceHandle* face_array, int length, Selection_Mode selection_mode){};
        static void default_cell_selection_function(OpenVolumeMesh::CellHandle* cell_array, int length, Selection_Mode selection_mode){};

        static void default_translate_operation_function(double x, double y, double z, Translation_Mode translation_mode){};
        static void default_rendering_operation_function(Rendering_Mode rendering_mode){};

    };

}


#endif //VOLUMESHOS_VOSWINDOW_H
