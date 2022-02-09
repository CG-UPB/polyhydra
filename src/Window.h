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
#include "rendering/shapes/Box.h"
#include "rendering/shapes/Cylinder.h"
#include "rendering/shapes/Sphere.h"
#include "panels/MeshView.h"
#include <mutex>
#include "panels/ToolBar.h"
#include "panels/MeshLayerView.h"
#include "settings/GlobalViewerSettings.h"

namespace vOS {

/*
 * Used by the programmer to visualize OVM Meshes using the vOS viewer
 * Vos is thread safe
 * Provides callback functions for events happening inside the viewer
 */

    // Forward Declration
    class MeshView;
    class Window
    {

    public:

        /*
         * Singleton Instance
         * Automatically instantiates instance if it doesn't exist yet
         */
        static Window &instance();

        // Renamed Classes for convenience
        // 3D Double Mesh
        using v3d = OpenVolumeMesh::GeometricPolyhedralMeshV3d;

        // Callback : Vertex selection / unselection
        typedef std::function<void(int mesh,int id, bool selected)> vertex_selection_callback;
        // Callback : Edge selection / unselection
        typedef std::function<void(int mesh,int id, bool selected)> edge_selection_callback;
        // Callback : Face selection / unselection
        typedef std::function<void(int mesh,int id, bool selected)> face_selection_callback;
        // Callback : Cell selection / unselection
        typedef std::function<void(int mesh,int id, bool selected)> cell_selection_callback;
        // Callback : Generally used void callback
        typedef std::function<void()> void_callback;

        // Meshes /////////////////////////////////////////////////////////////////////////////////////////////////////////////

        /*
         * Adds new OVM mesh to Vos
         * Returns index of Mesh for future identification
         */
        int add_mesh(OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3f> *mesh);
        /*
         * Removes Mesh identified by given index
         */
        void remove_mesh(int index);
        /*
         * Sets OVM Mesh at given index
         * Any Mesh occupying given index will be deleted
         */
        void set_mesh(OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3f> *mesh, int index = 0);

        /*
         * Sets the focused mesh inside the viewer
         */
        void set_mesh_focus(int index);
        /*
         * Gets the focused mesh inside the viewer
         */
        int get_mesh_focus();

        /*
         * Returns Vos MeshObject of Mesh that is focused inside the Viewer
         * Nullptr if no Mesh present in Viewer
         */
        MeshObject* get_focused_mesh_object(){return get_mesh_obj(m_focused_mesh);}

        /*
         * Return Vos MeshObject at given index
         * Nullptr if no MeshObject could be found
         */
        MeshObject *get_mesh_obj(int index);

        /*
         * True, if any MeshObject currently exists
         */
        bool has_mesh();

        /*
         * Returns all MeshObjects and their index
         */
        const std::unordered_map<int, MeshObject*>& get_mesh_list() { return m_mesh_objects; };

        /*
         * Deletes all Meshes
         */
        void remove_all_meshes();

        /*
         * Adds Shape Object
         * Returns unique Identifier
         */
        unsigned int add_shape(Shape* shape);
        /*
         * Removes Shape using its Identifier
         */
        void remove_shape(unsigned int id);
        /*
         * Removes all Shapes
         */
        void remove_all_shapes();


        //   Algorithm to Vos ////////////////////////////////////////////////////////////////////////////////////////////////

        /*
         * Applies rendering mode to mesh with given mesh_id
         * Prebuild modes are: mesh_phong, mesh_wireframe, mesh_flat, mesh_normal
         * To call custom shaders, simply add the .frag and .vert files in the res/shaders folder and use its name (without extension) as a paramater here
         */
        void set_mesh_rendering_mode(int mesh_id, std::string mode);
        /*
         * Applies rendering mode to the currently focused mesh
         * Prebuild modes are: mesh_phong, mesh_wireframe, mesh_flat, mesh_normal
         * To call custom shaders, simply add the .frag and .vert files in the res/shaders folder and use its name (without extension) as a paramater here
         */
        void set_mesh_rendering_mode( std::string mode);

        /*
         * Returns the actual rendering mode
         */
        std::string get_mesh_rendering_mode(int mesh_id);

        /*
         * Applies color to mesh with given mesh_id
         * To call custom shaders, simply add the .frag and .vert files in the res/shaders folder and use its name (without extension) as a paramater here
         */
        void set_mesh_color(int mesh_id, Color color);
        /*
         * Applies color to the currently focused mesh
         * To call custom shaders, simply add the .frag and .vert files in the res/shaders folder and use its name (without extension) as a paramater here
         */
        void set_mesh_color( Color color);

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
         * Applies visibility to the actually focused mesh.
         * A mesh that is not visible, will not be rendered in the Meshview class
         */
        void set_mesh_visibility(bool visible);

        /*
         * returns the visibility of the mesh with given mesh_id
         */
        bool get_mesh_visibility(int mesh_id);

        /*
         * Manually sets Peel level of a Mesh
         */
        void set_mesh_peel_level(int mesh_id, int level);

        /*
         * Manually sets Slice level of a Mesh
         */
        void set_mesh_slice_level(int mesh_id, float value);

        /*
         * Manually sets Cell Size of a Mesh
         */
        void set_cell_size(int mesh_id, float size);

        /*
         * Applies slice level to the actually focused mesh.
         */
        void set_mesh_slice_level(float slice_level);

        /*
         * Locks or unlocks the slice direction
         * If locked, the direction will be the last camera view direction
         * If unlocked, the direction will update each frame with the current camera view direction
         */
        void set_mesh_slice_locked(int mesh_id, bool locked);

        /*
         * Returns the slice level of a Mesh
         */
        float get_mesh_slice_level(int mesh_id);

        /*
         * Returns wether or not the slice direction is locked of a Mesh
         */
        bool get_mesh_slice_locked(int mesh_id);

        /*
         * Sets the position of a Mesh relative to the World Origin
         */
        void set_mesh_position(int mesh_id, float x, float y, float z);

        /*
         * Sets the uniform scale of a Mesh
         */
        void set_mesh_scale(int mesh_id, float scale);

        /*
         * Applies peel level to the actually focused mesh.
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
         * Applies cell size to the actually focused mesh.
         */
        void set_mesh_cell_size(float cell_size);

        /*
         * Returns the cell size of the mesh with given mesh_id
         */
        float get_mesh_cell_size(int mesh_id);

        /*
         * Indizes of all loaded Meshes
         */
        std::vector<int>* get_all_mesh_ids();

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
         * Example:  set_custom_imgui(std::bind(&ExampleClass::ui_method, this));
         */
        void set_custom_imgui(void_callback vc);

        /*
         * Sets custom function that will be called once Vos has initilized completely (glfw and imgui initialization and opening of the window)
         */
        void set_vos_initialized(void_callback vc) { m_vos_initialized = vc; };

        // Queueries //////////////////////////////////////////////////////////////////////////////////////////////////
        /*
        *  Returns true if Vos has been initialized and is working properly
        */
        bool is_ready();

        /*
         * Asks whether the Vos window has been closed by the user or not
         */
        bool is_closed();

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

        /*
         * Initializes (if not already) and opens the Vos Viewer Window
         * Will not do anything if it is already open
         */
        void open();

        /*
         * Ends the Vos Viewer Window manually
         */
        void end();

        // Camera //////////////////////////////////////////////////////////////////////////////////////////////////////

        /*
         * Sets the cameras world position relative to origin
         */
        void camera_set_position(float x, float y, float z);


        // IO //////////////////////////////////////////////////////////////////////////////////////////////////////////

        /*
         * Takes a screenshot of the current scene from the cameras perspective and saves the image to the given filepath
         */
        void take_screenshot(std::string filepath);

        // Opens Imgui File Dialogue
        void OpenFileDialogue();
        // Asks for a filepath from Imgui File Dialogue
        std::string GetFileDialoguePath();
        // True if File Dialogue is open
        bool FileDialogueOpen();
        // Closes Imgui File Dialogue
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
         * Unselects all elements of a given Mesh
         */
        void unselect_all_elements(int mesh);
        /*
         * Unselects all elements of every Mesh
         */
        void unselect_all_elements();

        // Generally Called when the User does anything to the Mesh (debug)
        void_callback m_on_general_update = default_callback_function;

        // Panels
        FileDialog* m_file_dialog;

        // Mutex and thread safety
        /// Set to guard GL when reading from and rendering our mesh
        std::mutex rendering_mutex;
    private:
        /*
         * Constructor
         */
        Window();

        /*
         * Deconstructor
         */
        ~Window();

        // Friend classes that need to lock/unlock rendering_mutex
        friend class MeshView;
        friend class MeshObject;
        friend class ToolBar;
        friend class Input;

        // Variables //////////////////////////////////////////////////////////////////////////////////////////////////

        // Counter to ensure every new Shape has a new ID
        unsigned int shape_id_counter = 0;

        // True when Vos Initialized
        bool m_initialized = false;
        // True when Vos has been opened
        bool m_window_open;
        // True when Vos is open and inside main rendering loop
        bool m_is_in_render_loop= false;

        // Custom UI Function
        // To avoid threading problems, we do not set the new custom UI function immediately but instead store it in this temporary function
        void_callback m_temporary_new_custom_ui_function;
        // True if a new UI function has been set
        bool m_new_custom_ui_function_set = false;

        // References //////////////////////////////////////////////////////////////////////////////////////////////////

        // Responsible for Mesh rendering
        MeshView* m_mesh_view;
        // Programmer-set custom ui function
        CustomUIPanel* m_custom_ui;
        // Responsible for Logging inside Vos
        LogWindow* m_log_window;
        // UI for Viewer Tools
        ToolBar* m_toolbar = ToolBar::getInstance();
        // UI for Meshes
        MeshLayerView* m_mesh_layer_view = MeshLayerView::getInstance();
        // Responsible for Imgui Communication
        ImguiRenderer *m_imgui_renderer;

        // Id of focused Mesh
        int m_focused_mesh = -1;
        // Amount of loaded Meshes
        int m_total_number_of_loaded_meshes = 0;
        // Indizes and their Meshes
        std::unordered_map<int, MeshObject*> m_mesh_objects;

        // Main Loop //

        // Initialize UI Panels
        void initPanels();

        // Render Frame
        void render();

        // Setup Vos
        void setup();

        // Close Vos
        void close();


        // Callback Interface //////////////////////////////////////////////////////////////////////////////////////////////////

        // Callback Functions //////////////////////////////////////////////////////////////////////////////////////////////////
        /// Default Empty Callback Function
        static void default_callback_function(){};

        // Default Empty Vertex Selection Function
        static void default_vertex_selection_function(int mesh, int id, bool selected) {};

        // Default Empty Edge Selection Function
        static void default_edge_selection_function(int mesh,int id, bool selected) {};

        // Default Empty Face Selection Function
        static void default_face_selection_function(int mesh,int id, bool selected) {};

        // Default Empty Cell Selection Function
        static void default_cell_selection_function(int mesh,int id, bool selected) {};

        // Called when Vos has initialized Imgui and Gl and is ready to render data
        void_callback m_vos_initialized = default_callback_function;

        // Called when a number of vertices have been selected
        vertex_selection_callback m_on_vertex_selection = default_vertex_selection_function;
        // Called when a number of edges have been selected
        edge_selection_callback m_on_edge_selection = default_edge_selection_function;
        // Called when a number of faces have been selected
        face_selection_callback m_on_face_selection = default_face_selection_function;
        // Called when a number of cells have been selected
        cell_selection_callback m_on_cell_selection = default_cell_selection_function;


    };

}


#endif //VOLUMESHOS_WINDOW_H
