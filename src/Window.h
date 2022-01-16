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
#include <unordered_map>
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
#include "panels/ToolBar.h"
#include "panels/MeshLayerView.h"
#include "settings/GlobalViewerSettings.h"

namespace vOS {

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
        enum class Selection_Mode {
            ADD, SUBTRACT, TOGGLE, SET
        };
        enum class Translation_Mode {
            ADD, SET, MULTIPLY
        };
        enum class Rendering_Mode {
            DRAW, HIDE, HIGHLIGHT
        };

        static Window &instance();

        // Renamed Classes for convenience
        using v3d = OpenVolumeMesh::GeometricPolyhedralMeshV3d;

        typedef std::function<void(int mesh,int id, bool selected)> vertex_selection_callback;
        typedef std::function<void(int mesh,int id, bool selected)> edge_selection_callback;
        typedef std::function<void(int mesh,int id, bool selected)> face_selection_callback;
        typedef std::function<void(int mesh,int id, bool selected)> cell_selection_callback;

        //typedef std::function<void(double x, double y, double z, Translation_Mode translation_mode)> operation_translation_callback;
        //typedef std::function<void(Rendering_Mode rendering_mode)> operation_rendering_callback;

        typedef std::function<void(const std::string &file)> file_dialog_callback;

        typedef std::function<void()> void_callback;

        // Meshes /////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void highlight_vertex(int mesh_id, OpenVolumeMesh::VertexHandle v_h, float red, float green, float blue, float alpha);
        void highlight_vertex(int mesh_id, OpenVolumeMesh::VertexHandle v_h, Color color);
        void remove_all_vertex_highlights();
        void remove_vertex_highlight(int mesh_id, OpenVolumeMesh::VertexHandle v_h);

        int add_mesh(OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3f> *mesh);
        void remove_mesh(int index);
        void set_mesh(OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3f> *mesh, int index = 0);

        void set_mesh_active(int index);
        int get_mesh_active();

        MeshObject* get_active_mesh_obj(){return get_mesh_obj(m_active_mesh);}
        MeshObject *get_mesh_obj(int index);

        bool has_mesh();

        const std::unordered_map<int, MeshObject*>& get_mesh_list() { return m_mesh_objects; };

        unsigned int add_shape(Shape* shape);
        void remove_shape(unsigned int id);

        //   Algorithm to Vos ////////////////////////////////////////////////////////////////////////////////////////////////

        /*
         * Applies rendering mode to mesh with given mesh_id
         * Prebuild modes are: mesh_phong, mesh_wireframe, mesh_flat, mesh_normal
         * To call custom shaders, simply add the .frag and .vert files in the res/shaders folder and use its name (without extension) as a paramater here
         */
        void set_mesh_rendering_mode(int mesh_id, std::string mode);
        /*
         * Applies rendering mode to the currently active mesh
         * Prebuild modes are: mesh_phong, mesh_wireframe, mesh_flat, mesh_normal
         * To call custom shaders, simply add the .frag and .vert files in the res/shaders folder and use its name (without extension) as a paramater here
         */
        void set_mesh_rendering_mode( std::string mode);

        /*
         * Returns the actual rendering mode for mesh with id = mesh_id
         */
        std::string get_mesh_rendering_mode(int mesh_id);

        /*
         * Applies color to mesh with given mesh_id
         * To call custom shaders, simply add the .frag and .vert files in the res/shaders folder and use its name (without extension) as a paramater here
         */
        void set_mesh_color(int mesh_id, Color m_color);
        /*
         * Applies color to the currently active mesh
         * To call custom shaders, simply add the .frag and .vert files in the res/shaders folder and use its name (without extension) as a paramater here
         */
        void set_mesh_color( Color m_color);

        /*
         * Returns the actual color for mesh with id = mesh_id
         */
        Color get_mesh_color(int mesh_id);

        /*
         * Applies visibility to the given mesh_id.
         * A mesh that is not visible, will not be rendered in the Meshview class
         */
        void set_mesh_visibility(int mesh_id,bool visible);

        /*
         * Applies visibility to the actual active mesh.
         * A mesh that is not visible, will not be rendered in the Meshview class
         */
        void set_mesh_visibility(bool visible);

        /*
         * returns the visibility of the mesh with given mesh_id
         */
        bool get_mesh_visibility(int mesh_id);

        /*
         * Applies slice level to the given mesh_id.
         */
        void set_mesh_slice_level(int mesh_id,float slice_level);

        /*
         * Applies slice level to the actual active mesh.
         */
        void set_mesh_slice_level(float slice_level);

        /*
         * returns the slice level of the mesh with given mesh_id
         */
        float get_mesh_slice_level(int mesh_id);

        /*
        * Applies peel level to the given mesh_id.
        */
        void set_mesh_peel_level(int mesh_id,int peel_level);

        /*
         * Applies peel level to the actual active mesh.
         */
        void set_mesh_peel_level(int peel_level);

        /*
         * returns the peel level of the mesh with given mesh_id
         */
        int get_mesh_peel_level(int mesh_id);

        /*
        * Applies cell size to the given mesh_id.
        */
        void set_mesh_cell_size(int mesh_id,float cell_size);

        /*
         * Applies cell size to the actual active mesh.
         */
        void set_mesh_cell_size(float cell_size);

        /*
         * returns the cell size of the mesh with given mesh_id
         */
        float get_mesh_cell_size(int mesh_id);


        /*
         * If set to false, the Vos Window will no longer interprete any inputs from Keys or Mouse
         * Useful for File Dialogues and similar processes in which you do not which the interface to change.
         */
        void set_intepret_input(bool interpret_input);

        /*
         * Rebinds the given GLFW key
         */
        void set_keybind_manual(int glfw_key_from, int glfw_key_to);

        // Callback Interface ///////////////////////////////////////////////////////////////////////////////////////////////

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
        void set_callback_vertex_selection(vertex_selection_callback vsc) { m_on_vertex_selection = vsc; };

        /*
         * Sets Callback Function which is called when the user performs a selection operation on edges
         */
        void set_callback_edge_selection(edge_selection_callback esc) { m_on_edge_selection = esc; };

        /*
         * Sets Callback Function which is called when the user performs a selection operation on faces
         */
        void set_callback_face_selection(face_selection_callback fsc) { m_on_face_selection = fsc; };

        /*
         * Sets Callback Function which is called when the user performs a selection operation on cells
         */
        void set_callback_cell_selection(cell_selection_callback csc) { m_on_cell_selection = csc; };


        void run();

        //void run(void_callback vc);


        // IO //////////////////////////////////////////////////////////////////////////////////////////////////////////

        void take_screenshot(std::string filename);

        void OpenFileDialogue();

        std::string GetFileDialoguePath();
        bool FileDialogueOpen();

        void EndFileDialogue();


        /*
         * Selects a given element (Face, Vertex, Edge) from given mesh
         * Element Types are:
         * 0 : Face
         * 1 : Vertex
         * 2 : Edge
         * 3 : Cell
         */
        void select_element(int mesh, int element_handle_id, int element_type);
        /*
         * Unselects a given element (Face, Vertex, Edge) from given mesh
         * Element Types are:
         * 0 : Face
         * 1 : Vertex
         * 2 : Edge
         * 3 : Cell
         */
        void unselect_element(int mesh, int element_handle_id, int element_type);
/*
         * Unselects all elements given Mesh has marked as selected
         */
        void unselect_all_elements(int mesh);
        /*
         * Unselects all elements any Mesh has marked as selected
         */
        void unselect_all_elements();
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

        // Panels
        FileDialog* m_file_dialog;

        // Mutex and thread safety
        /// Set to guard GL when reading from and rendering our mesh
        std::mutex rendering_mutex;
    private:
        Window();

        ~Window();

        friend class MeshView;
        friend class MeshObject;
        friend class ToolBar;

        // Debugging //////////////////////////////////////////////////////////////////////////////////////////////////
        bool m_pause_toggled = false;

        // Variables //////////////////////////////////////////////////////////////////////////////////////////////////

        unsigned int shape_id_counter = 0;

        bool m_initialized = false;
        bool m_window_open;
        bool m_window_paused = false;

        // Custom UI Function
        void_callback m_temporary_new_custom_ui_function;
        bool m_new_custom_ui_function_set = false;




        // References //////////////////////////////////////////////////////////////////////////////////////////////////

        MeshView* m_mesh_view;
        CustomUIPanel* m_custom_ui;
        LogWindow* m_log_window;
        ToolBar* m_toolbar = ToolBar::getInstance();
        MeshLayerView* m_mesh_layer_view = MeshLayerView::getInstance();

        ImguiRenderer *m_imgui_renderer;

        int m_active_mesh = -1;
        int m_total_number_of_loaded_meshes = 0;
        std::unordered_map<int, MeshObject*> m_mesh_objects;

        void calculate_selection_offsets();

        // Main Loop //
        void initPanels();

        void render();

        void open();

        void close();


        // Callback Interface //////////////////////////////////////////////////////////////////////////////////////////////////

        // Callback Functions //////////////////////////////////////////////////////////////////////////////////////////////////
        /// Default Empty Callback Function
        static void default_callback_function(){};

        static void default_vertex_selection_function(int mesh, int id, bool selected) {};

        static void default_edge_selection_function(int mesh,int id, bool selected) {};

        static void default_face_selection_function(int mesh,int id, bool selected) {};

        static void default_cell_selection_function(int mesh,int id, bool selected) {};

        static void
        default_translate_operation_function(double x, double y, double z, Translation_Mode translation_mode) {};

        static void default_rendering_operation_function(Rendering_Mode rendering_mode) {};



    };

}


#endif //VOLUMESHOS_WINDOW_H
