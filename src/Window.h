#pragma once
//
// Created by jan on 15.10.21.
//

#ifndef VOLUMESHOS_WINDOW_H
#define VOLUMESHOS_WINDOW_H

#include <iostream>
#include <OpenVolumeMesh/Mesh/HexahedralMesh.hh>
#include <OpenVolumeMesh/Mesh/PolyhedralMesh.hh>
#include <string>
#include <list>
#include <map>
#include <functional>
#include "ImguiRenderer.h"
#include "mesh/MeshObject.h"
#include "panels/CustomUIPanel.h"
#include "panels/LogWindow.h"
#include "panels/FileDialog.h"
#include "rendering/shapes/Shape.h"
#include "rendering/shapes/Box.h"
#include "panels/MeshView.h"
#include <mutex>

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
class MeshView;
    class Window
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

        static Window& instance();

        // Renamed Classes for convenience
        using v3f = OpenVolumeMesh::GeometricPolyhedralMeshV3f;

        typedef std::tuple<OpenVolumeMesh::VertexHandle, float, float, float, float, bool> operation_set_highlight;
        typedef std::tuple<Shape*, unsigned int, bool> operation_shape;
        typedef std::function<void(OpenVolumeMesh::VertexHandle* vertices_array, int length, Selection_Mode selection_mode)> vertex_selection_callback;
        typedef std::function<void(OpenVolumeMesh::EdgeHandle* edges_array, int length, Selection_Mode selection_mode)> edge_selection_callback;
        typedef std::function<void(OpenVolumeMesh::FaceHandle* faces_array, int length, Selection_Mode selection_mode)> face_selection_callback;
        typedef std::function<void(OpenVolumeMesh::CellHandle* cells_array, int length, Selection_Mode selection_mode)> cell_selection_callback;

        //typedef std::function<void(double x, double y, double z, Translation_Mode translation_mode)> operation_translation_callback;
        //typedef std::function<void(Rendering_Mode rendering_mode)> operation_rendering_callback;

        typedef std::function<void(const std::string& file)> file_dialog_callback;

        typedef std::function<void()> void_callback;

        // Meshes /////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void highlight_vertex(OpenVolumeMesh::VertexHandle v_h, float red, float green, float blue, float alpha);
        void highlight_vertex(OpenVolumeMesh::VertexHandle v_h, Color color);
        void remove_all_vertex_highlights();
        void remove_vertex_highlight(OpenVolumeMesh::VertexHandle v_h);

        void set_mesh(OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3f> *mesh, int index=0);

        MeshObject& get_mesh_obj(int index = 0);

        unsigned int add_shape(Shape* shape);
        void remove_shape(unsigned int id);

        //   Algorithm to Vos ////////////////////////////////////////////////////////////////////////////////////////////////


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

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////// Callback Interface ///////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::string get_loaded_file_name() {return m_loaded_file_path;};
        void set_loaded_file_path_name(std::string path) { m_loaded_file_path = path;};

        /*
         * Sets custom function that will be called along other Vos UI panel elements
         * Updates will be adopted after the current rendering frame has ended
         */
        void set_custom_imgui(void_callback vc);

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

        bool is_running();

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


        void run();

        //void run(void_callback vc);
        // Custom ImGui Methods

        static bool ShowFileDialog(std::string& path, const std::string& extension = ".ovm");
        FileDialog* get_file_dialog(){return m_file_dialog;}

        // User Input Reactions //////////////////////////////////////////////////////////////////////////////////////////////////

        // Called when a number of vertices have been selected
        vertex_selection_callback m_on_vertex_selection = default_vertex_selection_function;
        // Called when a number of edges have been selected
        edge_selection_callback m_on_edge_selection = default_edge_selection_function;
        // Called when a number of faces have been selected
        face_selection_callback m_on_face_selection = default_face_selection_function;
        // Called when a number of cells have been selected
        cell_selection_callback m_on_cell_selection = default_cell_selection_function;

        //operation_translation_callback m_on_translate_operation = default_translate_operation_function;
        //operation_rendering_callback m_on_rendering_operation = default_rendering_operation_function;
        // Generally Called when the User does anything to the Mesh (debug)
        void_callback m_on_general_update = default_callback_function;

    private:
        Window();
        ~Window();

        // Debugging //////////////////////////////////////////////////////////////////////////////////////////////////
        bool m_pause_toggled = false;

        // Variables //////////////////////////////////////////////////////////////////////////////////////////////////

        std::string m_loaded_file_path = "";
        unsigned int shape_id_counter = 0;

        bool m_initialized = false;
        bool m_window_open;
        bool m_window_paused = false;

        // Custom UI Function
        void_callback m_temporary_new_custom_ui_function;
        bool m_new_custom_ui_function_set = false;

        // Mutex and thread safety
        /// Set to guard GL when reading from and rendering our mesh
        std::mutex rendering_mutex;

        // Operations /////////////////////////////////////////////////////////////////////////////////////////////////

        std::list<operation_set_highlight> operation_list_vertex_highlights;
        std::list<operation_shape> operation_list_shapes;

        // References //////////////////////////////////////////////////////////////////////////////////////////////////

        // Panels
        FileDialog* m_file_dialog;
        MenuBar* m_menu_bar;
        MeshView* m_mesh_view;
        CustomUIPanel* m_custom_ui;
        LogWindow* m_log_window;

        v3f *m_mesh_reference;
        ImguiRenderer *m_imgui_renderer;

        std::map<int, MeshObject*> m_mesh_objects;
        MeshObject m_mesh_obj;

        MenuBar* get_menu_bar(){return m_menu_bar;}

        // Main Loop //
        void initPanels();

        void render();

        void open();

        void close();
        // Callback Interface //////////////////////////////////////////////////////////////////////////////////////////////////

        // Callback Functions //////////////////////////////////////////////////////////////////////////////////////////////////
        /// Default Empty Callback Function
        static void default_callback_function(){};

        static void default_vertex_selection_function(OpenVolumeMesh::VertexHandle* vertices_array, int length, Selection_Mode selection_mode){};
        static void default_edge_selection_function(OpenVolumeMesh::EdgeHandle* edge_array, int length, Selection_Mode selection_mode){};
        static void default_face_selection_function(OpenVolumeMesh::FaceHandle* face_array, int length, Selection_Mode selection_mode){};
        static void default_cell_selection_function(OpenVolumeMesh::CellHandle* cell_array, int length, Selection_Mode selection_mode){};

        static void default_translate_operation_function(double x, double y, double z, Translation_Mode translation_mode){};
        static void default_rendering_operation_function(Rendering_Mode rendering_mode){};

    };

}


#endif //VOLUMESHOS_WINDOW_H
