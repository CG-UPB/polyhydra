#pragma once

#include "vospch.h"

namespace volumeshOS
{
    struct VMesh;
    struct VShape;
    struct VArrow;

    // General

    void initialize();

    // Open volumeshOS context and start rendering
    void open();

    // Close volumeshOS context
    void close();


    /* Callbacks */

    // Define programmer user interface here
    void on_gui_render(const std::function<void()>& callback);


    // Called each frame a cell is hovered
    void on_cell_hover(const std::function<void(const VMesh, OpenVolumeMesh::CellHandle)>& callback);

    // Called each frame a face is hovered
    void on_face_hover(const std::function<void(const VMesh, OpenVolumeMesh::FaceHandle)>& callback);

    // Called each frame a halfface is hovered
    void on_halfface_hover(const std::function<void(const VMesh, OpenVolumeMesh::HalfFaceHandle)>& callback);

    // Called each frame an edge is hovered
    void on_edge_hover(const std::function<void(const VMesh, OpenVolumeMesh::EdgeHandle)>& callback);

    // Called each frame a vertex is hovered
    void on_vertex_hover(const std::function<void(const VMesh, OpenVolumeMesh::VertexHandle)>& callback);


    // Called once when a cell is selected (clicked)
    void on_cell_select(const std::function<void(const VMesh, OpenVolumeMesh::CellHandle)>& callback);

    // Called once when a face is selected (clicked)
    void on_face_select(const std::function<void(const VMesh, OpenVolumeMesh::FaceHandle)>& callback);

    // Called once when a halfface is selected (clicked)
    void on_halfface_select(const std::function<void(const VMesh, OpenVolumeMesh::HalfFaceHandle)>& callback);

    // Called once when an edge is selected (clicked)
    void on_edge_select(const std::function<void(const VMesh, OpenVolumeMesh::EdgeHandle)>& callback);

    // Called once when a vertex is selected (clicked)
    void on_vertex_select(const std::function<void(const VMesh, OpenVolumeMesh::VertexHandle)>& callback);

    // Called once when any position is selected
    void on_position_select(const std::function<void(float, float, float)>& callback);


    /* VMesh manipulation */

    // Add mesh from ovm instance, return new wrapper
    [[nodiscard]] VMesh load(OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>* instance, const char* name = nullptr);

    // Add mesh from file path, return new wrapper
    [[nodiscard]] VMesh load(const std::string& path, const char* name = nullptr);

    // Add mesh from file path, return new wrapper
    [[nodiscard]] VMesh load(const char* path, const char* name = nullptr);

    // Add mesh from file path, return new wrapper
    [[nodiscard]] VMesh load_from_dialog(const std::string& title, const char* name = nullptr);


    // Set mesh from ovm instance without generating a new id
    void update(const VMesh& mesh, OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>* instance);

    // Set mesh from file path without generating a new id
    void update(const VMesh& mesh, const std::string& path);

    // Set mesh from file path without generating a new id
    void update(const VMesh& mesh, const char* path);


    // Remove all meshes from volumeshOS
    void clear();

    // Remove mesh from volumeshOS
    void clear(const VMesh& mesh);


    // Returns the OpenVolumeMesh instance of a mesh
    [[nodiscard]] OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>* get_ovm(const VMesh& mesh);

    // Returns a list of all loaded meshes
    [[nodiscard]] std::vector<VMesh> get_meshes();


    // Load a configuration file for a mesh
    void load_configuration(const VMesh& mesh, const std::string& path);

    // Save a configuration file for a mesh
    void save_configuration(const VMesh& mesh, const std::string& path);


    // Set color for all meshes (all cells and halffaces)
    template<typename Vec4T>
    void set_color(const Vec4T& color);

    // Set color for one mesh (all cells and halffaces)
    template<typename Vec4T>
    void set_color(const VMesh& mesh, const Vec4T& color);

    // Set color for one cell of a mesh
    template<typename Vec4T>
    void set_color(const VMesh& mesh, OpenVolumeMesh::CellHandle cell, const Vec4T& color);

    // Set color for one face (both halffaces) of a mesh
    template<typename Vec4T>
    void set_color(const VMesh& mesh, OpenVolumeMesh::FaceHandle face, const Vec4T& color);

    // Set color for one halfface of a mesh
    template<typename Vec4T>
    void set_color(const VMesh& mesh, OpenVolumeMesh::HalfFaceHandle halfface, const Vec4T& color);

    // Set color for one edge of a mesh
    template<typename Vec4T>
    void set_color(const VMesh& mesh, OpenVolumeMesh::EdgeHandle edge, const Vec4T& color);

    // Set color for one vertex of a mesh
    template<typename Vec4T>
    void set_color(const VMesh& mesh, OpenVolumeMesh::VertexHandle vertex, const Vec4T& color);


    // Returns the color of the mesh
    template<typename Vec4T>
    [[nodiscard]] Vec4T get_color(const VMesh& mesh);

    // Returns the color of a cell from a given mesh
    template<typename Vec4T>
    [[nodiscard]] Vec4T get_color(const VMesh& mesh, OpenVolumeMesh::CellHandle cell);

    // Returns the color of a halfface from a given mesh
    template<typename Vec4T>
    [[nodiscard]] Vec4T get_color(const VMesh& mesh, OpenVolumeMesh::HalfFaceHandle halfface);

    // Returns the color of an edge from a given mesh
    template<typename Vec4T>
    [[nodiscard]] Vec4T get_color(const VMesh& mesh, OpenVolumeMesh::EdgeHandle edge);

    // Returns the color of a vertex from a given mesh
    template<typename Vec4T>
    [[nodiscard]] Vec4T get_color(const VMesh& mesh, OpenVolumeMesh::VertexHandle vertex);


    // Set name of a mesh
    void set_name(const VMesh& mesh, const std::string& name);

    // Returns the name of a mesh
    [[nodiscard]] const std::string& get_name(const VMesh& mesh);


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

    // Set the position of a mesh
    template<typename Vec3T>
    void set_position(const VMesh& mesh, const Vec3T& position);

    // Set the scale of a mesh
    void set_scale(const VMesh& mesh, float scale);

    // Set the rotation of a mesh using euler angles
    void set_rotation(const VMesh& mesh, float x, float y, float z);

    // Set the rotation of a mesh
    template<typename Vec3T>
    void set_rotation(const VMesh& mesh, const Vec3T& rotation);

    // Resets the rotation of a mesh
    void reset_rotation(const VMesh& mesh);

    // Set the slice factor for a mesh. 0 (no slicing) to 1 (full slicing of the mesh)
    void set_slice_factor(const VMesh& mesh, float factor);

    // Lock the direction of the slice plane
    void set_slice_lock(const VMesh& mesh, bool lock);

    // Set the peel level for a given mesh. 0 (no peel) up to the total number of depth layers in the mesh
    void set_peel_level(const VMesh& mesh, float level);

    // Set the rounding factor for each cell of a mesh. 0 (no rounding) to 1 (full rounding)
    void set_cell_rounding(const VMesh& mesh, float rounding);

    // Activate roundings
    void activate_rounding(const VMesh& mesh, bool rounding);

    // Set the cell size of a given mesh. 0 (infinitely small) to 1 (original size)
    void set_cell_size(const VMesh& mesh, float size);

    // Set the cell visibility of a mesh
    void set_visibility(const VMesh& mesh, OpenVolumeMesh::CellHandle cell, bool visible);

    // Set the visibility of a mesh
    void set_visibility(const VMesh& mesh, bool visible);

    // set currently active mesh
    void set_focused_mesh(VMesh mesh);


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
    template<typename Vec3T>
    [[nodiscard]] Vec3T get_position(const VMesh& mesh);

    // Returns the scale of a mesh
    [[nodiscard]] float get_scale(const VMesh& mesh);

    // Returns the rotation of a mesh using euler angles
    template<typename Vec3T>
    [[nodiscard]] Vec3T get_rotation(const VMesh& mesh);

    // Returns the slice factor for a mesh. 0 (no slicing) to 1 (full slicing of the mesh)
    [[nodiscard]] float get_slice_factor(const VMesh& mesh);

    // Returns the lock direction of the slice plane
    [[nodiscard]] bool get_slice_lock(const VMesh& mesh);

    // Returns the peel level for a given mesh. 0 (no peel) up to the total number of depth layers in the mesh
    [[nodiscard]] float get_peel_level(const VMesh& mesh);

    // Returns the number of cell depth layers of the mesh
    [[nodiscard]] int get_max_peel_depth(const VMesh& mesh);

    // Returns the rounding factor for each cell of a mesh. 0 (no rounding) to 1 (full rounding)
    [[nodiscard]] float get_cell_rounding(const VMesh& mesh);

    // Returns the cell size of a given mesh. 0 (infinitely small) to 1 (original size)
    [[nodiscard]] float get_cell_size(const VMesh& mesh);

    // Returns the visibility of a cell
    [[nodiscard]] bool get_visibility(const VMesh& mesh, OpenVolumeMesh::CellHandle cell);

    // Returns the visibility of the mesh
    [[nodiscard]] bool get_visibility(const VMesh& mesh);

    // Returns the current mesh in focus
    [[nodiscard]] VMesh get_focused_mesh();

    // Returns true if the mesh is a valid handle
    [[nodiscard]] bool is_valid(const VMesh& mesh);


    /* Shapes */

    template<typename ShapeType>
    [[nodiscard]] ShapeType add_shape();

    template<typename ShapeType>
    [[nodiscard]] ShapeType add_shape(const VMesh& mesh);

    template<typename ShapeType>
    [[nodiscard]] ShapeType add_shape(const VMesh& mesh, OpenVolumeMesh::CellHandle cell);

    void remove_shape(const VShape& shape);

    void remove_shapes();

    void set_position(const VShape& shape, float x, float y, float z);

    template<typename Vec3T>
    void set_position(const VShape& shape, const Vec3T& position);

    void set_direction(const VShape& shape, float axis_x, float axis_y, float axis_z, float angle = 0.0f);

    template<typename Vec3T>
    void set_direction(const VShape& shape, const Vec3T& axis, float angle = 0.0f);

    void set_scale(const VShape& shape, float scalar);

    void set_scale(const VShape& shape, float x, float y, float z);

    template<typename Vec3T>
    void set_scale(const VShape& shape, const Vec3T& scale);

    template<typename Vec4T>
    void set_color(const VShape& shape, const Vec4T& color);

    template<typename Vec3T>
    [[nodiscard]] Vec3T get_position(const VShape& shape);

    template<typename Vec3T>
    [[nodiscard]] Vec3T get_scale(const VShape& shape);

    void set_tip_height(const VArrow& shape, float tip_height);

    void set_base_width(const VArrow& shape, float base_width);

    /* Camera */

    // Set the position of the camera
    void set_camera_position(float x, float y, float z);

    // Set the position of the camera
    template<typename Vec3T>
    void set_camera_position(const Vec3T& position);

    // Set the view direction of the camera
    void set_camera_view_direction(float x, float y, float z);

    // Set the view direction of the camera
    template<typename Vec3T>
    void set_camera_view_direction(const Vec3T& direction);

    // Set the camera mode
    void set_camera_mode(Mode mode);

    // Set the camera view direction to point to a given mesh
    void focus_camera(const VMesh& mesh);


    /* Global settings */

    // Set the direction of the light in the scene
    void set_light_direction(float x, float y, float z);

    // Set the direction of the light in the scene
    template<typename Vec3T>
    void set_light_direction(const Vec3T& direction);

    // Set the viewport background color
    template<typename Vec4T>
    void set_background_color(const Vec4T& color);


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

    // Log a message
    void log(const std::string& message);


    struct VShape
    {
        explicit VShape(int id = -1) : m_id(id)
        {}

        inline void remove() const
        {
            volumeshOS::remove_shape(*this);
        }

        inline void set_position(float x, float y, float z) const
        {
            volumeshOS::set_position(*this, x, y, z);
        }

        template<typename Vec3T>
        inline void set_position(const Vec3T& position) const
        {
            volumeshOS::set_position<Vec3T>(*this, position);
        }

        inline void set_direction(float axis_x, float axis_y, float axis_z, float angle = 0.0f) const
        {
            volumeshOS::set_direction(*this, axis_x, axis_y, axis_z, angle);
        }

        template<typename Vec3T>
        inline void set_direction(const Vec3T& axis, float angle = 0.0f) const
        {
            volumeshOS::set_direction<Vec3T>(*this, axis, angle);
        }

        inline void set_scale(float x, float y, float z) const
        {
            volumeshOS::set_scale(*this, x, y, z);
        }

        inline void set_scale(float scalar) const
        {
            volumeshOS::set_scale(*this, scalar);
        }

        template<typename Vec3T>
        inline void set_scale(const Vec3T& scale) const
        {
            volumeshOS::set_scale<Vec3T>(*this, scale);
        }

        template<typename Vec4T>
        inline void set_color(const Vec4T& color) const
        {
            volumeshOS::set_color<Vec4T>(*this, color);
        }

        template<typename Vec3T>
        [[nodiscard]] inline Vec3T get_position() const
        {
            return volumeshOS::get_position<Vec3T>(*this);
        }

        template<typename Vec3T>
        [[nodiscard]] inline Vec3T get_scale() const
        {
            return volumeshOS::get_scale<Vec3T>(*this);
        }

        // Returns the internal volumeshOS id of this shape
        [[nodiscard]] int get_id() const
        {
            return m_id;
        }

    private:
        int m_id;
    };

    struct VSphere : public VShape
    {
        using VShape::VShape;
    };

    struct VCylinder : public VShape
    {
        using VShape::VShape;
    };

    struct VBox : public VShape
    {
        using VShape::VShape;
    };

    struct VCone : public VShape
    {
        using VShape::VShape;
    };

    struct VArrow : public VShape
    {
        using VShape::VShape;

        inline void set_tip_height(float tip_height) const
        {
            volumeshOS::set_tip_height(*this, tip_height);
        }

        inline void set_base_width(float base_width) const
        {
            volumeshOS::set_base_width(*this, base_width);
        }
    };

    // Wrapper class to use meshes in an object-oriented way
    struct VMesh
    {
        explicit VMesh(int id = -1) : m_id(id)
        {}

        // Set mesh from ovm instance without generating a new id
        inline void update(OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>* instance) const
        {
            volumeshOS::update(*this, instance);
        }

        // Set mesh from file path without generating a new id
        inline void update(const std::string& path) const
        {
            volumeshOS::update(*this, path);
        }

        // Remove mesh from volumeshOS
        inline void clear() const
        {
            volumeshOS::clear(*this);
        }


        // Returns the OpenVolumeMesh instance of this mesh
        [[nodiscard]] inline OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>* get_ovm() const
        {
            return volumeshOS::get_ovm(*this);
        }

        template<typename ShapeType>
        [[nodiscard]] inline ShapeType add_shape() const
        {
            return volumeshOS::add_shape<ShapeType>(*this);
        }

        template<typename ShapeType>
        [[nodiscard]] inline ShapeType add_shape(OpenVolumeMesh::CellHandle cell) const
        {
            return volumeshOS::add_shape<ShapeType>(*this, cell);
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
        template<typename Vec4T>
        inline void set_color(const Vec4T& color) const
        {
            volumeshOS::set_color<Vec4T>(*this, color);
        }

        // Set color for one cell of a mesh
        template<typename Vec4T>
        inline void set_color(OpenVolumeMesh::CellHandle cell, const Vec4T& color) const
        {
            volumeshOS::set_color<Vec4T>(*this, cell, color);
        }

        // Set color for one face (both halffaces) of a mesh
        template<typename Vec4T>
        inline void set_color(OpenVolumeMesh::FaceHandle face, const Vec4T& color) const
        {
            volumeshOS::set_color<Vec4T>(*this, face, color);
        }

        // Set color for one halfface of a mesh
        template<typename Vec4T>
        inline void set_color(OpenVolumeMesh::HalfFaceHandle halfface, const Vec4T& color) const
        {
            volumeshOS::set_color<Vec4T>(*this, halfface, color);
        }

        // Set color for one edge of a mesh
        template<typename Vec4T>
        inline void set_color(OpenVolumeMesh::EdgeHandle edge, const Vec4T& color) const
        {
            volumeshOS::set_color<Vec4T>(*this, edge, color);
        }

        // Set color for one vertex of a mesh
        template<typename Vec4T>
        inline void set_color(OpenVolumeMesh::VertexHandle vertex, const Vec4T& color) const
        {
            volumeshOS::set_color<Vec4T>(*this, vertex, color);
        }


        // Returns the color of the mesh
        template<typename Vec4T>
        [[nodiscard]] inline Vec4T get_color() const
        {
            return volumeshOS::get_color<Vec4T>(*this);
        }

        // Returns the color of a cell from a given mesh
        template<typename Vec4T>
        [[nodiscard]] inline Vec4T get_color(OpenVolumeMesh::CellHandle cell) const
        {
            return volumeshOS::get_color<Vec4T>(*this, cell);
        }

        // Returns the color of a halfface from a given mesh
        template<typename Vec4T>
        [[nodiscard]] inline Vec4T get_color(OpenVolumeMesh::HalfFaceHandle halfface) const
        {
            return volumeshOS::get_color<Vec4T>(*this, halfface);
        }

        // Returns the color of an edge from a given mesh
        template<typename Vec4T>
        [[nodiscard]] inline Vec4T get_color(OpenVolumeMesh::EdgeHandle edge) const
        {
            return volumeshOS::get_color<Vec4T>(*this, edge);
        }

        // Returns the color of a vertex from a given mesh
        template<typename Vec4T>
        [[nodiscard]] inline Vec4T get_color(OpenVolumeMesh::VertexHandle vertex) const
        {
            return volumeshOS::get_color<Vec4T>(*this, vertex);
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

        // Set the position of a mesh
        template<typename Vec3T>
        inline void set_position(const Vec3T& position) const
        {
            volumeshOS::set_position<Vec3T>(*this, position);
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

        // Set the rotation of a mesh using euler angles
        template<typename Vec3T>
        inline void set_rotation(const Vec3T& rotation) const
        {
            volumeshOS::set_rotation<Vec3T>(*this, rotation);
        }

        inline void reset_rotation() const
        {
            volumeshOS::reset_rotation(*this);
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

        // Set currently active mesh
        inline void set_focused_mesh(VMesh mesh) const
        {
            volumeshOS::set_focused_mesh(*this);
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

        // Set the name of a mesh
        inline void set_name(const std::string& name) const
        {
            volumeshOS::set_name(*this, name);
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
        template<typename Vec3T>
        [[nodiscard]] inline Vec3T get_position() const
        {
            return volumeshOS::get_position<Vec3T>(*this);
        }

        // Returns the scale of a mesh
        [[nodiscard]] inline float get_scale() const
        {
            return volumeshOS::get_scale(*this);
        }

        // Returns the rotation of a mesh using euler angles
        template<typename Vec3T>
        [[nodiscard]] inline Vec3T get_rotation() const
        {
            return volumeshOS::get_rotation<Vec3T>(*this);
        }

        // Returns the slice factor for a mesh. 0 (no slicing) to 1 (full slicing of the mesh)
        [[nodiscard]] inline float get_slice_factor() const
        {
            return volumeshOS::get_slice_factor(*this);
        }

        // Returns the lock direction of the slice plane
        [[nodiscard]] inline bool get_slice_lock() const
        {
            return volumeshOS::get_slice_lock(*this);
        }

        // Returns the peel level for a given mesh. 0 (no peel) up to the total number of depth layers in the mesh
        [[nodiscard]] inline float get_peel_level() const
        {
            return volumeshOS::get_peel_level(*this);
        }

        // Returns the number of cell depth layers of the mesh
        [[nodiscard]] inline int get_max_peel_depth() const
        {
            return volumeshOS::get_max_peel_depth(*this);
        }

        // Returns the rounding factor for each cell of a mesh. 0 (no rounding) to 1 (full rounding)
        [[nodiscard]] inline float get_cell_rounding() const
        {
            return volumeshOS::get_cell_rounding(*this);
        }

        inline void activate_rounding(bool rounding) const
        {
            return volumeshOS::activate_rounding(*this, rounding);
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

        // Returns the name of the mesh
        [[nodiscard]] inline const std::string& get_name() const
        {
            return volumeshOS::get_name(*this);
        }

        // Returns true if the mesh is a valid handle
        [[nodiscard]] inline bool is_valid() const
        {
            return volumeshOS::is_valid(*this);
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
