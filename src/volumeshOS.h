#pragma once

#include "vospch.h"

namespace volumeshOS
{
    struct VMesh;

    // General

    // Open volumeshOS context and start rendering
    void open();

    // Close volumeshOS context
    void close();


    // Callbacks

    // Define programmer user interface here
    void on_gui_render(std::function<void()> callback);


    // Called each frame a cell is hovered
    void on_cell_hover(std::function<void(const VMesh, OpenVolumeMesh::CellHandle)> callback);

    // Called each frame a face is hovered
    void on_face_hover(std::function<void(const VMesh, OpenVolumeMesh::FaceHandle)> callback);

    // Called each frame a halfface is hovered
    void on_halfface_hover(std::function<void(const VMesh, OpenVolumeMesh::HalfFaceHandle)> callback);

    // Called each frame an edge is hovered
    void on_edge_hover(std::function<void(const VMesh, OpenVolumeMesh::EdgeHandle)> callback);

    // Called each frame a vertex is hovered
    void on_vertex_hover(std::function<void(const VMesh, OpenVolumeMesh::VertexHandle)> callback);


    // Called once when a cell is selected (clicked)
    void on_cell_select(std::function<void(const VMesh, OpenVolumeMesh::CellHandle)> callback);

    // Called once when a face is selected (clicked)
    void on_face_select(std::function<void(const VMesh, OpenVolumeMesh::FaceHandle)> callback);

    // Called once when a halfface is selected (clicked)
    void on_halfface_select(std::function<void(const VMesh, OpenVolumeMesh::HalfFaceHandle)> callback);

    // Called once when an edge is selected (clicked)
    void on_edge_select(std::function<void(const VMesh, OpenVolumeMesh::EdgeHandle)> callback);

    // Called once when a vertex is selected (clicked)
    void on_vertex_select(std::function<void(const VMesh, OpenVolumeMesh::VertexHandle)> callback);

    // Called once when any position is selected
    void on_position_select(std::function<void(float, float, float)> callback);

    // VMesh manipulation


    // Add mesh from ovm instance, return new wrapper
    [[nodiscard]] VMesh load(OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>* instance);

    // Add mesh from file path, return new wrapper
    [[nodiscard]] VMesh load(const std::string& path);


    // Set mesh from ovm instance without generating a new id
    void load(const VMesh& mesh, OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>* instance);

    // Set mesh from file path without generating a new id
    void load(const VMesh& mesh, const std::string& path);


    // Remove all meshes from volumeshOS
    void clear();

    // Remove mesh from volumeshOS
    void clear(const VMesh& mesh);


    // Load a configuration file for a mesh
    void load_configuration(const VMesh& mesh, const std::string& path);

    // Save a configuration file for a mesh
    void save_configuration(const VMesh& mesh, const std::string& path);


    // Set color for all meshes (all cells and halffaces)
    void set_color(const Color& color);

    // Set color for one mesh (all cells and halffaces)
    void set_color(const VMesh& mesh, const Color& color);

    // Set color for one cell of a mesh
    void set_color(const VMesh& mesh, OpenVolumeMesh::CellHandle cell, const Color& color);

    // Set color for one face (both halffaces) of a mesh
    void set_color(const VMesh& mesh, OpenVolumeMesh::FaceHandle face, const Color& color);

    // Set color for one halfface of a mesh
    void set_color(const VMesh& mesh, OpenVolumeMesh::HalfFaceHandle halfface, const Color& color);

    // Set color for one edge of a mesh
    void set_color(const VMesh& mesh, OpenVolumeMesh::EdgeHandle edge, const Color& color);

    // Set color for one vertex of a mesh
    void set_color(const VMesh& mesh, OpenVolumeMesh::VertexHandle vertex, const Color& color);


    // Returns the color of the mesh
    [[nodiscard]] Color get_color(const VMesh& mesh);

    // Returns the color of a cell from a given mesh
    [[nodiscard]] Color get_color(const VMesh& mesh, OpenVolumeMesh::CellHandle cell);

    // Returns the color of a face from a given mesh
    [[nodiscard]] Color get_color(const VMesh& mesh, OpenVolumeMesh::FaceHandle face);

    // Returns the color of a halfface from a given mesh
    [[nodiscard]] Color get_color(const VMesh& mesh, OpenVolumeMesh::HalfFaceHandle halfface);

    // Returns the color of an edge from a given mesh
    [[nodiscard]] Color get_color(const VMesh& mesh, OpenVolumeMesh::EdgeHandle edge);

    // Returns the color of a vertex from a given mesh
    [[nodiscard]] Color get_color(const VMesh& mesh, OpenVolumeMesh::VertexHandle vertex);


    // Select one cell of a mesh
    void select(const VMesh& mesh, OpenVolumeMesh::CellHandle cell);

    // Select one face (both halffaces) of a mesh
    void select(const VMesh& mesh, OpenVolumeMesh::FaceHandle face);

    // Select one halfface of a mesh
    void select(const VMesh& mesh, OpenVolumeMesh::HalfFaceHandle halfface);

    // Select one edge of a mesh
    void select(const VMesh& mesh, OpenVolumeMesh::EdgeHandle edge);

    // Select one vertex of a mesh
    void select(const VMesh& mesh, OpenVolumeMesh::VertexHandle vertex);


    // Deselect one cell of a mesh
    void deselect(const VMesh& mesh, OpenVolumeMesh::CellHandle cell);

    // Deselect one face (both halffaces) of a mesh
    void deselect(const VMesh& mesh, OpenVolumeMesh::FaceHandle face);

    // Deselect one halfface of a mesh
    void deselect(const VMesh& mesh, OpenVolumeMesh::HalfFaceHandle halfface);

    // Deselect one edge of a mesh
    void deselect(const VMesh& mesh, OpenVolumeMesh::EdgeHandle edge);

    // Deselect one vertex of a mesh
    void deselect(const VMesh& mesh, OpenVolumeMesh::VertexHandle vertex);

    // Deselect all entities of a mesh
    void reset_selection(const VMesh& mesh);


    // Set the ambient term for the phong lighting model of a mesh
    void set_ambient(const VMesh& mesh, float ambient);

    // Set the diffuse term for the phong lighting model of a mesh
    void set_diffuse(const VMesh& mesh, float diffuse);

    // Set the specular term for the phong lighting model of a mesh
    void set_specular(const VMesh& mesh, float specular);

    // Set the specular coefficient for the phong lighting model of a mesh
    void set_specular_coefficient(const VMesh& mesh, float coefficient);

    // Set parameters for the phong lighting model of a mesh
    void set_phong(const VMesh& mesh, float ambient, float diffuse, float specular, float coefficient);

    // Set the position of a mesh
    void set_position(const VMesh& mesh, float x, float y, float z);

    // Set the scale of a mesh
    void set_scale(const VMesh& mesh, float scale);

    // Set the rotation of a mesh using euler angles
    void set_rotation(const VMesh& mesh, float x, float y, float z);

    // Set the slice factor for a mesh. 0 (no slicing) to 1 (full slicing of the mesh)
    void set_slice_factor(const VMesh& mesh, float factor);

    // Lock the direction of the slice plane
    void set_slice_lock(const VMesh& mesh, bool lock);

    // Set the peel level for a given mesh. 0 (no peel) up to the total number of depth layers in the mesh
    void set_peel_level(const VMesh& mesh, float level);

    // Set the rounding factor for each cell of a mesh. 0 (no rounding) to 1 (full rounding)
    void set_cell_rounding(const VMesh& mesh, float rounding);

    // Set the cell size of a given mesh. 0 (infinitely small) to 1 (original size)
    void set_cell_size(const VMesh& mesh, float size);

    // Set the cell visibility of a mesh
    void set_visibility(const VMesh& mesh, OpenVolumeMesh::CellHandle cell, bool visible);

    // Set the visibility of a mesh
    void set_visibility(const VMesh& mesh, bool visible);

    // Reset the visibility so that all cells are visible
    void reset_visibility(const VMesh& mesh);

    // Isolate a single cell of a mesh, making it the only visible cell
    void isolate(const VMesh& mesh, OpenVolumeMesh::CellHandle cell);

    // Hide a cell of a mesh
    void hide(const VMesh& mesh, OpenVolumeMesh::CellHandle cell);


    // Returns the ambient term for the phong lighting model of a mesh
    [[nodiscard]] float get_ambient(const VMesh& mesh);

    // Returns the diffuse term for the phong lighting model of a mesh
    [[nodiscard]] float get_diffuse(const VMesh& mesh);

    // Returns the specular term for the phong lighting model of a mesh
    [[nodiscard]] float get_specular(const VMesh& mesh);

    // Returns the specular coefficient for the phong lighting model of a mesh
    [[nodiscard]] float get_specular_coefficient(const VMesh& mesh);

    // Returns the position of a mesh
    [[nodiscard]] std::tuple<float, float, float> get_position(const VMesh& mesh);

    // Returns the scale of a mesh
    [[nodiscard]] float get_scale(const VMesh& mesh);

    // Returns the rotation of a mesh using euler angles
    [[nodiscard]] std::tuple<float, float, float> get_rotation(const VMesh& mesh);

    // Returns the slice factor for a mesh. 0 (no slicing) to 1 (full slicing of the mesh)
    [[nodiscard]] float get_slice_factor(const VMesh& mesh);

    // Returns the lock direction of the slice plane
    [[nodiscard]] float get_slice_lock(const VMesh& mesh);

    // Returns the peel level for a given mesh. 0 (no peel) up to the total number of depth layers in the mesh
    [[nodiscard]] float get_peel_level(const VMesh& mesh);

    // Returns the rounding factor for each cell of a mesh. 0 (no rounding) to 1 (full rounding)
    [[nodiscard]] float get_cell_rounding(const VMesh& mesh);

    // Returns the cell size of a given mesh. 0 (infinitely small) to 1 (original size)
    [[nodiscard]] float get_cell_size(const VMesh& mesh);

    // Returns the visibility of a cell
    [[nodiscard]] bool get_visibility(const VMesh& mesh, OpenVolumeMesh::CellHandle cell);

    // Returns the visibility of the mesh
    [[nodiscard]] bool get_visibility(const VMesh& mesh);


    /* Camera */

    // Set the position of the camera
    void set_camera_position(float x, float y, float z);

    // Set the view direction of the camera
    void set_camera_view_direction(float x, float y, float z);

    // Set the camera mode
    void set_camera_mode(Mode mode);

    // Set the camera view direction to point to a given mesh
    void focus(const VMesh& mesh);


    /* Global settings */

    // Set the direction of the light in the scene
    void set_light_direction(float x, float y, float z);

    // Set the viewport background color
    void set_background_color(const Color& color);


    /* Miscellaneous */

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



    // Wrapper class to use meshes in an object-oriented way
    struct VMesh
    {
        explicit VMesh(int id = -1) : m_id(id)
        {}

        // Set mesh from ovm instance without generating a new id
        inline void load(OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>* instance) const
        {
            volumeshOS::load(*this, instance);
        }

        // Set mesh from file path without generating a new id
        inline void load(const std::string& path) const
        {
            volumeshOS::load(*this, path);
        }

        // Remove mesh from volumeshOS
        inline void clear() const
        {
            volumeshOS::clear(*this);
        }


        // Load a configuration file for a mesh
        inline void load_configuration(const std::string& path) const
        {
            volumeshOS::load_configuration(*this, path);
        }

        // Save a configuration file for a mesh
        inline void save_configuration(const std::string& path) const
        {
            volumeshOS::save_configuration(*this, path);
        }


        // Set color for one mesh (all cells and halffaces)
        inline void set_color(const Color& color) const
        {
            volumeshOS::set_color(*this, color);
        }

        // Set color for one cell of a mesh
        inline void set_color(OpenVolumeMesh::CellHandle cell, const Color& color) const
        {
            volumeshOS::set_color(*this, cell, color);
        }

        // Set color for one face (both halffaces) of a mesh
        inline void set_color(OpenVolumeMesh::FaceHandle face, const Color& color) const
        {
            volumeshOS::set_color(*this, face, color);
        }

        // Set color for one halfface of a mesh
        inline void set_color(OpenVolumeMesh::HalfFaceHandle halfface, const Color& color) const
        {
            volumeshOS::set_color(*this, halfface, color);
        }

        // Set color for one edge of a mesh
        inline void set_color(OpenVolumeMesh::EdgeHandle edge, const Color& color) const
        {
            volumeshOS::set_color(*this, edge, color);
        }

        // Set color for one vertex of a mesh
        inline void set_color(OpenVolumeMesh::VertexHandle vertex, const Color& color) const
        {
            volumeshOS::set_color(*this, vertex, color);
        }


        // Returns the color of the mesh
        [[nodiscard]] inline Color get_color() const
        {
            return volumeshOS::get_color(*this);
        }

        // Returns the color of a cell from a given mesh
        [[nodiscard]] inline Color get_color(OpenVolumeMesh::CellHandle cell) const
        {
            return volumeshOS::get_color(*this, cell);
        }

        // Returns the color of a face from a given mesh
        [[nodiscard]] inline Color get_color(OpenVolumeMesh::FaceHandle face) const
        {
            return volumeshOS::get_color(*this, face);
        }

        // Returns the color of a halfface from a given mesh
        [[nodiscard]] inline Color get_color(OpenVolumeMesh::HalfFaceHandle halfface) const
        {
            return volumeshOS::get_color(*this, halfface);
        }

        // Returns the color of an edge from a given mesh
        [[nodiscard]] inline Color get_color(OpenVolumeMesh::EdgeHandle edge) const
        {
            return volumeshOS::get_color(*this, edge);
        }

        // Returns the color of a vertex from a given mesh
        [[nodiscard]] inline Color get_color(OpenVolumeMesh::VertexHandle vertex) const
        {
            return volumeshOS::get_color(*this, vertex);
        }


        // Select one cell of a mesh
        inline void select(OpenVolumeMesh::CellHandle cell) const
        {
            volumeshOS::select(*this, cell);
        }

        // Select one face (both halffaces) of a mesh
        inline void select(OpenVolumeMesh::FaceHandle face) const
        {
            volumeshOS::select(*this, face);
        }

        // Select one halfface of a mesh
        inline void select(OpenVolumeMesh::HalfFaceHandle halfface) const
        {
            volumeshOS::select(*this, halfface);
        }

        // Select one edge of a mesh
        inline void select(OpenVolumeMesh::EdgeHandle edge) const
        {
            volumeshOS::select(*this, edge);
        }

        // Select one vertex of a mesh
        inline void select(OpenVolumeMesh::VertexHandle vertex) const
        {
            volumeshOS::select(*this, vertex);
        }


        // Deselect one cell of a mesh
        inline void deselect(OpenVolumeMesh::CellHandle cell) const
        {
            volumeshOS::deselect(*this, cell);
        }

        // Deselect one face (both halffaces) of a mesh
        inline void deselect(OpenVolumeMesh::FaceHandle face) const
        {
            volumeshOS::deselect(*this, face);
        }

        // Deselect one halfface of a mesh
        inline void deselect(OpenVolumeMesh::HalfFaceHandle halfface) const
        {
            volumeshOS::deselect(*this, halfface);
        }

        // Deselect one edge of a mesh
        inline void deselect(OpenVolumeMesh::EdgeHandle edge) const
        {
            volumeshOS::deselect(*this, edge);
        }

        // Deselect one vertex of a mesh
        inline void deselect(OpenVolumeMesh::VertexHandle vertex) const
        {
            volumeshOS::deselect(*this, vertex);
        }

        // Deselect all entities of a mesh
        inline void reset_selection() const
        {
            volumeshOS::reset_selection(*this);
        }


        // Set the ambient term for the phong lighting model of a mesh
        inline void set_ambient(float ambient) const
        {
            volumeshOS::set_ambient(*this, ambient);
        }

        // Set the diffuse term for the phong lighting model of a mesh
        inline void set_diffuse(float diffuse) const
        {
            volumeshOS::set_diffuse(*this, diffuse);
        }

        // Set the specular term for the phong lighting model of a mesh
        inline void set_specular(float specular) const
        {
            volumeshOS::set_specular(*this, specular);
        }

        // Set the specular coefficient for the phong lighting model of a mesh
        inline void set_specular_coefficient(float coefficient) const
        {
            volumeshOS::set_specular_coefficient(*this, coefficient);
        }

        // Set parameters for the phong lighting model of a mesh
        inline void set_phong(float ambient, float diffuse, float specular, float coefficient) const
        {
            volumeshOS::set_phong(*this, ambient, diffuse, specular, coefficient);
        }

        // Set the position of a mesh
        inline void set_position(float x, float y, float z) const
        {
            volumeshOS::set_position(*this, x, y, z);
        }

        // Set the scale of a mesh
        inline void set_scale(float scale) const
        {
            volumeshOS::set_scale(*this, scale);
        }

        // Set the rotation of a mesh using euler angles
        inline void set_rotation(float x, float y, float z) const
        {
            volumeshOS::set_rotation(*this, x, y, z);
        }

        // Set the slice factor for a mesh. 0 (no slicing) to 1 (full slicing of the mesh)
        inline void set_slice_factor(float factor) const
        {
            volumeshOS::set_slice_factor(*this, factor);
        }

        // Lock the direction of the slice plane
        inline void set_slice_lock(bool lock) const
        {
            volumeshOS::set_slice_lock(*this, lock);
        }

        // Set the peel level for a given mesh. 0 (no peel) up to the total number of depth layers in the mesh
        inline void set_peel_level(float level) const
        {
            volumeshOS::set_peel_level(*this, level);
        }

        // Set the rounding factor for each cell of a mesh. 0 (no rounding) to 1 (full rounding)
        inline void set_cell_rounding(float rounding) const
        {
            volumeshOS::set_cell_rounding(*this, rounding);
        }

        // Set the cell size of a given mesh. 0 (infinitely small) to 1 (original size)
        inline void set_cell_size(float size) const
        {
            volumeshOS::set_cell_size(*this, size);
        }

        // Set the cell visibility of a mesh
        inline void set_visibility(OpenVolumeMesh::CellHandle cell, bool visible) const
        {
            volumeshOS::set_visibility(*this, cell, visible);
        }

        // Set the visibility of a mesh
        inline void set_visibility(bool visible) const
        {
            volumeshOS::set_visibility(*this, visible);
        }

        // Reset the visibility so that all cells are visible
        inline void reset_visibility() const
        {
            volumeshOS::reset_visibility(*this);
        }

        // Isolate a single cell of a mesh, making it the only visible cell
        inline void isolate(OpenVolumeMesh::CellHandle cell) const
        {
            volumeshOS::isolate(*this, cell);
        }

        // Hide a cell of a mesh
        inline void hide(OpenVolumeMesh::CellHandle cell) const
        {
            volumeshOS::hide(*this, cell);
        }


        // Returns the ambient term for the phong lighting model of a mesh
        [[nodiscard]] inline float get_ambient() const
        {
            return volumeshOS::get_ambient(*this);
        }

        // Returns the diffuse term for the phong lighting model of a mesh
        [[nodiscard]] inline float get_diffuse() const
        {
            return volumeshOS::get_diffuse(*this);
        }

        // Returns the specular term for the phong lighting model of a mesh
        [[nodiscard]] inline float get_specular() const
        {
            return volumeshOS::get_specular(*this);
        }

        // Returns the specular coefficient for the phong lighting model of a mesh
        [[nodiscard]] inline float get_specular_coefficient() const
        {
            return volumeshOS::get_specular_coefficient(*this);
        }

        // Returns the position of a mesh
        [[nodiscard]] inline std::tuple<float, float, float> get_position() const
        {
            return volumeshOS::get_position(*this);
        }

        // Returns the scale of a mesh
        [[nodiscard]] inline float get_scale() const
        {
            return volumeshOS::get_scale(*this);
        }

        // Returns the rotation of a mesh using euler angles
        [[nodiscard]] inline std::tuple<float, float, float> get_rotation() const
        {
            return volumeshOS::get_rotation(*this);
        }

        // Returns the slice factor for a mesh. 0 (no slicing) to 1 (full slicing of the mesh)
        [[nodiscard]] inline float get_slice_factor() const
        {
            return volumeshOS::get_slice_factor(*this);
        }

        // Returns the lock direction of the slice plane
        [[nodiscard]] inline float get_slice_lock() const
        {
            return volumeshOS::get_slice_lock(*this);
        }

        // Returns the peel level for a given mesh. 0 (no peel) up to the total number of depth layers in the mesh
        [[nodiscard]] inline float get_peel_level() const
        {
            return volumeshOS::get_peel_level(*this);
        }

        // Returns the rounding factor for each cell of a mesh. 0 (no rounding) to 1 (full rounding)
        [[nodiscard]] inline float get_cell_rounding() const
        {
            return volumeshOS::get_cell_rounding(*this);
        }

        // Returns the cell size of a given mesh. 0 (infinitely small) to 1 (original size)
        [[nodiscard]] inline float get_cell_size() const
        {
            return volumeshOS::get_cell_size(*this);
        }

        // Returns the visibility of a cell
        [[nodiscard]] inline bool get_visibility(OpenVolumeMesh::CellHandle cell) const
        {
            return volumeshOS::get_visibility(*this, cell);
        }

        // Returns the visibility of the mesh
        [[nodiscard]] inline bool get_visibility() const
        {
            return volumeshOS::get_visibility(*this);
        }

        // Returns the internal volumeshOS id of this mesh
        [[nodiscard]] int get_id() const
        {
            return m_id;
        }

    private:
        int m_id;
    };

} // namespace volumeshOS
