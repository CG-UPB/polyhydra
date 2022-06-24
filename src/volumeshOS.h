#pragma once

#include "vospch.h"
#include "util/Types.h"

namespace volumeshOS
{
    // General

    // Open volumeshOS context and start rendering
    void open();

    // Close volumeshOS context
    void close();


    // Callbacks

    // Define programmer user interface here
    void on_gui_render(std::function<void()> callback);


    // Called each frame a cell is hovered
    void on_cell_hover(std::function<void(const Mesh, OpenVolumeMesh::CellHandle)> callback);

    // Called each frame a face is hovered
    void on_face_hover(std::function<void(const Mesh, OpenVolumeMesh::FaceHandle)> callback);

    // Called each frame a halfface is hovered
    void on_halfface_hover(std::function<void(const Mesh, OpenVolumeMesh::HalfFaceHandle)> callback);

    // Called each frame an edge is hovered
    void on_edge_hover(std::function<void(const Mesh, OpenVolumeMesh::EdgeHandle)> callback);

    // Called each frame a vertex is hovered
    void on_vertex_hover(std::function<void(const Mesh, OpenVolumeMesh::VertexHandle)> callback);


    // Called once when a cell is selected (clicked)
    void on_cell_select(std::function<void(const Mesh, OpenVolumeMesh::CellHandle)> callback);

    // Called once when a face is selected (clicked)
    void on_face_select(std::function<void(const Mesh, OpenVolumeMesh::FaceHandle)> callback);

    // Called once when a halfface is selected (clicked)
    void on_halfface_select(std::function<void(const Mesh, OpenVolumeMesh::HalfFaceHandle)> callback);

    // Called once when an edge is selected (clicked)
    void on_edge_select(std::function<void(const Mesh, OpenVolumeMesh::EdgeHandle)> callback);

    // Called once when a vertex is selected (clicked)
    void on_vertex_select(std::function<void(const Mesh, OpenVolumeMesh::VertexHandle)> callback);

    // Called once when any position is selected
    void on_position_select(std::function<void(float, float, float)> callback);

    // Mesh manipulation


    // Add mesh from ovm instance, return new wrapper
    [[nodiscard]] const Mesh load(OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>* instance);

    // Add mesh from file path, return new wrapper
    [[nodiscard]] const Mesh load(const std::string& path);


    // Set mesh from ovm instance without generating a new id
    void load(const Mesh& mesh, OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>* instance);

    // Set mesh from file path without generating a new id
    void load(const Mesh& mesh, const std::string& path);


    // Remove all meshes from volumeshOS
    void clear();

    // Remove mesh from volumeshOS
    void clear(const Mesh& mesh);


    // Load a configuration file for a mesh
    void load_configuration(const Mesh& mesh, const std::string& path);

    // Save a configuration file for a mesh
    void save_configuration(const Mesh& mesh, const std::string& path);


    // Set color for all meshes (all cells and halffaces)
    void set_color(const Color& color);

    // Set color for one mesh (all cells and halffaces)
    void set_color(const Mesh& mesh, const Color& color);

    // Set color for one cell of a mesh
    void set_color(const Mesh& mesh, OpenVolumeMesh::CellHandle cell, const Color& color);

    // Set color for one face (both halffaces) of a mesh
    void set_color(const Mesh& mesh, OpenVolumeMesh::FaceHandle face, const Color& color);

    // Set color for one halfface of a mesh
    void set_color(const Mesh& mesh, OpenVolumeMesh::HalfFaceHandle halfface, const Color& color);

    // Set color for one edge of a mesh
    void set_color(const Mesh& mesh, OpenVolumeMesh::EdgeHandle edge, const Color& color);

    // Set color for one vertex of a mesh
    void set_color(const Mesh& mesh, OpenVolumeMesh::VertexHandle vertex, const Color& color);


    // Returns the color of the mesh
    [[nodiscard]] Color get_color(const Mesh& mesh);

    // Returns the color of a cell from a given mesh
    [[nodiscard]] Color get_color(const Mesh& mesh, OpenVolumeMesh::CellHandle cell);

    // Returns the color of a face from a given mesh
    [[nodiscard]] Color get_color(const Mesh& mesh, OpenVolumeMesh::FaceHandle face);

    // Returns the color of a halfface from a given mesh
    [[nodiscard]] Color get_color(const Mesh& mesh, OpenVolumeMesh::HalfFaceHandle halfface);

    // Returns the color of an edge from a given mesh
    [[nodiscard]] Color get_color(const Mesh& mesh, OpenVolumeMesh::EdgeHandle edge);

    // Returns the color of a vertex from a given mesh
    [[nodiscard]] Color get_color(const Mesh& mesh, OpenVolumeMesh::VertexHandle vertex);


    // Select one cell of a mesh
    void select(const Mesh& mesh, OpenVolumeMesh::CellHandle cell);

    // Select one face (both halffaces) of a mesh
    void select(const Mesh& mesh, OpenVolumeMesh::FaceHandle face);

    // Select one halfface of a mesh
    void select(const Mesh& mesh, OpenVolumeMesh::HalfFaceHandle halfface);

    // Select one edge of a mesh
    void select(const Mesh& mesh, OpenVolumeMesh::EdgeHandle edge);

    // Select one vertex of a mesh
    void select(const Mesh& mesh, OpenVolumeMesh::VertexHandle vertex);


    // Deselect one cell of a mesh
    void deselect(const Mesh& mesh, OpenVolumeMesh::CellHandle cell);

    // Deselect one face (both halffaces) of a mesh
    void deselect(const Mesh& mesh, OpenVolumeMesh::FaceHandle face);

    // Deselect one halfface of a mesh
    void deselect(const Mesh& mesh, OpenVolumeMesh::HalfFaceHandle halfface);

    // Deselect one edge of a mesh
    void deselect(const Mesh& mesh, OpenVolumeMesh::EdgeHandle edge);

    // Deselect one vertex of a mesh
    void deselect(const Mesh& mesh, OpenVolumeMesh::VertexHandle vertex);

    // Deselect all entities of a mesh
    void reset_selection(const Mesh& mesh);


    // Set the ambient term for the phong lighting model of a mesh
    void set_ambient(const Mesh& mesh, float ambient);

    // Set the diffuse term for the phong lighting model of a mesh
    void set_diffuse(const Mesh& mesh, float diffuse);

    // Set the specular term for the phong lighting model of a mesh
    void set_specular(const Mesh& mesh, float specular);

    // Set the specular coefficient for the phong lighting model of a mesh
    void set_specular_coefficient(const Mesh& mesh, float coefficient);

    // Set parameters for the phong lighting model of a mesh
    void set_phong(const Mesh& mesh, float ambient, float diffuse, float specular, float coefficient);

    // Set the position of a mesh
    void set_position(const Mesh& mesh, float x, float y, float z);

    // Set the scale of a mesh
    void set_scale(const Mesh& mesh, float scale);

    // Set the rotation of a mesh using euler angles
    void set_rotation(const Mesh& mesh, float x, float y, float z);

    // Set the slice factor for a mesh. 0 (no slicing) to 1 (full slicing of the mesh)
    void set_slice_factor(const Mesh& mesh, float factor);

    // Lock the direction of the slice plane
    void set_slice_lock(const Mesh& mesh, bool lock);

    // Set the peel level for a given mesh. 0 (no peel) up to the total number of depth layers in the mesh
    void set_peel_level(const Mesh& mesh, float level);

    // Set the rounding factor for each cell of a mesh. 0 (no rounding) to 1 (full rounding)
    void set_cell_rounding(const Mesh& mesh, float rounding);

    // Set the cell size of a given mesh. 0 (infinitely small) to 1 (original size)
    void set_cell_size(const Mesh& mesh, float size);

    // Set the cell visibility of a mesh
    void set_visibility(const Mesh& mesh, OpenVolumeMesh::CellHandle cell, bool visible);

    // Set the visibility of a mesh
    void set_visibility(const Mesh& mesh, bool visible);

    // Reset the visibility so that all cells are visible
    void reset_visibility(const Mesh& mesh);

    // Isolate a single cell of a mesh, making it the only visible cell
    void isolate(const Mesh& mesh, OpenVolumeMesh::CellHandle cell);

    // Hide a cell of a mesh
    void hide(const Mesh& mesh, OpenVolumeMesh::CellHandle cell);


    // Returns the ambient term for the phong lighting model of a mesh
    [[nodiscard]] float get_ambient(const Mesh& mesh);

    // Returns the diffuse term for the phong lighting model of a mesh
    [[nodiscard]] float get_diffuse(const Mesh& mesh);

    // Returns the specular term for the phong lighting model of a mesh
    [[nodiscard]] float get_specular(const Mesh& mesh);

    // Returns the specular coefficient for the phong lighting model of a mesh
    [[nodiscard]] float get_specular_coefficient(const Mesh& mesh);

    // Returns the position of a mesh
    [[nodiscard]] std::tuple<float, float, float> get_position(const Mesh& mesh);

    // Returns the scale of a mesh
    [[nodiscard]] float get_scale(const Mesh& mesh);

    // Returns the rotation of a mesh using euler angles
    [[nodiscard]] std::tuple<float, float, float> get_rotation(const Mesh& mesh);

    // Returns the slice factor for a mesh. 0 (no slicing) to 1 (full slicing of the mesh)
    [[nodiscard]] float get_slice_factor(const Mesh& mesh);

    // Returns the lock direction of the slice plane
    [[nodiscard]] float get_slice_lock(const Mesh& mesh);

    // Returns the peel level for a given mesh. 0 (no peel) up to the total number of depth layers in the mesh
    [[nodiscard]] float get_peel_level(const Mesh& mesh);

    // Returns the rounding factor for each cell of a mesh. 0 (no rounding) to 1 (full rounding)
    [[nodiscard]] float get_cell_rounding(const Mesh& mesh);

    // Returns the cell size of a given mesh. 0 (infinitely small) to 1 (original size)
    [[nodiscard]] float get_cell_size(const Mesh& mesh);

    // Returns the visibility of a cell
    [[nodiscard]] bool get_visibility(const Mesh& mesh, OpenVolumeMesh::CellHandle cell);

    // Returns the visibility of the mesh
    [[nodiscard]] bool get_visibility(const Mesh& mesh);


    // Camera

    // Set the position of the camera
    void set_camera_position(float x, float y, float z);

    // Set the view direction of the camera
    void set_camera_view_direction(float x, float y, float z);

    // Set the camera mode
    void set_camera_mode(Mode mode);

    // Set the camera view direction to point to a given mesh
    void focus(const Mesh& mesh);


    // Global settings

    // Set the direction of the light in the scene
    void set_light_direction(float x, float y, float z);

    // Set the viewport background color
    void set_background_color(const Color& color);


    // Miscellaneous

    // Export the current viewport as png
    void export_image();

    // Export the current viewport as png to a specified file
    void export_image(const std::string& path);

    // Determines if input events should be registered and handled
    void ignore_inputs(bool ignore);

    // Open a file dialog with a given title. Returns either nullptr if no file is selected, or the file path
    const char* file_dialog(const std::string& title);

    // Set the color theme of volumeshOS
    void set_theme(Theme theme);
}
