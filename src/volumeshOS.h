#pragma once

#include "vospch.h"
#include "util/Enums.h"

namespace volumeshOS
{
    struct VMesh;
    struct VShape;
    struct VArrow;

    void init();

    // Open volumeshOS context and start rendering
    void open();

    // Close volumeshOS context
    void close();


    /* SETTINGS */

    /* Callbacks */

    // Define programmer user interface here
    void on_gui_render(const std::function<void()>& callback);

    /**
     * Convert Mesh from OpenVolumeMesh into a new VMesh object
     * @param instance OVM mesh
     * @param name name that is shown in the mesh list
     * @return Vmesh object
     */
    template<typename KernelType>
    [[nodiscard]] VMesh load(const OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d, KernelType>* instance, const char* name = nullptr);

    /**
     * Convert path to OVM mesh into a new VMesh object
     * @param path path to OVM mesh
     * @param name name that is shown in the mesh list
     * @return Vmesh object
     */
    [[nodiscard]] VMesh load(const std::string& path, const char* name = nullptr);

    /**
     * Convert path to OVM mesh into a new VMesh object
     * @param path path to OVM mesh
     * @param name name that is shown in the mesh list
     * @return Vmesh object
     */
    [[nodiscard]] VMesh load(const char* path, const char* name = nullptr);

    /**
     * Opens a file dialog with a given title.
     * Convert path to OVM mesh into a new VMesh object
     * @param title title for file dialog
     * @param name name that is shown in the mesh list
     * @return Vmesh object
     */
    [[nodiscard]] VMesh load_from_dialog(const std::string& title, const char* name = nullptr);

    /**
     * Set given mesh into focus. Some operations only occur on the focused mesh.
     * @param mesh VMesh
     */
    void set_focused_mesh(const VMesh mesh);

    VMesh get_focused_mesh();

    /**
     * Set name for mesh
     * @param mesh VMesh
     * @param name name string
     */
    void set_name(const VMesh& mesh, const std::string& name);

    const std::string& get_name(const VMesh& mesh);


    /* GENEREAL */

    /**
     * Set Rendering Mode.
     * @param mode
     * Phong: Rendering using phong shading \n
     * Flat : Rendering using flat shading \n
     * Lines: Rendering edges aka wireframe \n
     * Points: Rendering vertices aka points \n
     */
    void set_rendering_mode(RenderingMode mode);

    RenderingMode get_rendering_mode();

    // Set Background color
    template<typename Vec3T>
    void set_sky_color(const Vec3T& color);

    template<typename Vec3T>
    Vec3T& get_sky_color();


    /* SELECTION */

    /**
     * Set Selection Mode. Select entity with left mouse button.
     * Hovering an entity with selection active highlights them.
     * @param mode mode
     * OFF: Selection off
     * VERTEX: Select Vertices
     * EDGE: Select Edges
     * FACE: Select Faces
     * ALL: Select everything at once
     */
    void set_selection_mode(SelectionMode mode);

    SelectionMode get_selection_mode();

    /**
     * Specify a function that gets executed when a cell is hovered
     * @param callback function to execute
     */
    void on_cell_hover(const std::function<void(const VMesh, OpenVolumeMesh::CellHandle)>& callback);

    /**
     * Specify a function that gets executed when a face is hovered
     * @param callback function to execute
     */
    void on_face_hover(const std::function<void(const VMesh, OpenVolumeMesh::FaceHandle)>& callback);

    /**
     * Specify a function that gets executed when a halfface is hovered
     * @param callback function to execute
     */
    void on_halfface_hover(const std::function<void(const VMesh, OpenVolumeMesh::HalfFaceHandle)>& callback);

    /**
     * Specify a function that gets executed when an edge  is hovered
     * @param callback function to execute
     */
    void on_edge_hover(const std::function<void(const VMesh, OpenVolumeMesh::EdgeHandle)>& callback);

    /**
     * Specify a function that gets executed when a vertex is hovered
     * @param callback function to execute
     */
    void on_vertex_hover(const std::function<void(const VMesh, OpenVolumeMesh::VertexHandle)>& callback);

    /**
     * Specify a function that gets executed when a cell is selected
     * @param callback function to execute
     */
    void on_cell_select(const std::function<void(const VMesh, OpenVolumeMesh::CellHandle)>& callback);

    /**
     * Specify a function that gets executed when a face is selected
     * @param callback function to execute
     */
    void on_face_select(const std::function<void(const VMesh, OpenVolumeMesh::FaceHandle)>& callback);

    /**
     * Specify a function that gets executed when a halfface is selected
     * @param callback function to execute
     */
    void on_halfface_select(const std::function<void(const VMesh, OpenVolumeMesh::HalfFaceHandle)>& callback);

    /**
     * Specify a function that gets executed when an edge is selected
     * @param callback function to execute
     */
    void on_edge_select(const std::function<void(const VMesh, OpenVolumeMesh::EdgeHandle)>& callback);

    /**
     * Specify a function that gets executed when a vertex is selected
     * @param callback function to execute
     */
    void on_vertex_select(const std::function<void(const VMesh, OpenVolumeMesh::VertexHandle)>& callback);

    /**
     * Specify a function that gets executed when a position is selected
     * @param callback function to execute
     */
    void on_position_select(const std::function<void(float, float, float)>& callback);

    /* CAMERA */

    /**
     * Set camera mode. Switch mode with 'M'.
     * @param mode mode
     * ORBIT: Camera orbits around a target point. Doubleclicking any point sets mode to ORBIT and clicked poitn to target.
     * FLY: Camera moves around freely.
     */
    void set_camera_mode(CameraMode mode);

    CameraMode get_camera_mode();

    /**
     * Set camera postion in world coordinates.
     * @param x X coordinate
     * @param y Y coordinate
     * @param z Z coordinate
     */
    void set_camera_position(float x, float y, float z);

    /**
     * Set camera postion in world coordinates.
     * @param position coordinate vector
     */
    template<typename Vec3T>
    void set_camera_position(const Vec3T& position);

    template<typename Vec3T>
    Vec3T get_camera_position();

    /**
     * Set camera target in world coordinates.
     * @param x X coordinate
     * @param y Y coordinate
     * @param z Z coordinate
     */
    void set_camera_target(float x, float y, float z);

    /**
     * Set camera target in world coordinates.
     * @param target coordinate vector
     */
    template<typename Vec3T>
    void set_camera_target(const Vec3T& target);

    template<typename Vec3T>
    Vec3T get_camera_target();

    /**
     * Focus camera on given mesh. Uses mesh center of gravity as target point.
     * @param target coordinate vector
     */
    void focus_camera_on_mesh(const VMesh& mesh);

    /**
     * Set camera field of view.
     * @param fov value between 1 and 90 (degree).
     */
    void set_camera_fov(float fov);

    float get_fov();

    /* LIGHT */

    /**
     * Set light direction in world coordinates.
     * @param x X coordinate
     * @param y Y coordinate
     * @param z Y coordinate
     */
    void set_light_direction(float x, float y, float z);

    /**
     * Set light direction in world coordinates.
     * @param direction coordinate vector
     */
    template<typename Vec3T>
    void set_light_direction(const Vec3T& direction);

    template<typename Vec3T>
    Vec3T get_light_direction();


    /* POST PROCESSING */

    /**
     * Set gamma value for post processing.
     * @param gamma value
     */
    void set_gamma(float gamma);

    float get_gamma();

    /**
     * Set saturation value for post processing.
     * @param saturation value
     */
    void set_saturation(float saturation);

    float get_saturation();

    /**
     * Set contrast value for post processing.
     * @param contrast value
     */
    void set_contrast(float contrast);

    float get_contrast();


    /* GROUND */

    /**
     * Enable/Disable ground. The ground receives shadow.
     * @param ground
     */
    void use_ground(bool ground);

    bool is_using_ground();

    /**
     * Enable/Disable grid. The grid can lay on top of the ground.
     * @param ground
     */
    void use_grid(bool grid);

    bool is_using_grid();

    /**
     * Set ground color.
     * @param color vector in range [0,1]
     */
    template<typename Vec3T>
    void set_ground_color(const Vec3T& color);

    template<typename Vec3T>
    Vec3T get_ground_color();

    /**
     * Set grid color.
     * @param color vector in range [0,1]
     */
    template<typename Vec3T>
    void set_grid_color(const Vec3T& color);

    template<typename Vec3T>
    Vec3T get_grid_color();

    /**
     * Set ground height.
     * @param height value in range world coordinates.
     */
    void set_ground_height(float height);

    float get_gound_height();


    /* SHADOWS */

    /**
     * Enable/Disable shadows.
     * @param shadows
     */
    void use_shadows(bool shadows);

    bool is_using_shadows();


    /* AMBIENT OCCLUSION */

    /**
     * Enable/Disable ambient occlusion.
     * @param shadows
     */
    void use_ambient_occlusion( bool ssao);

    bool is_using_ambient_occlusion();

    /**
    * Enable/Disable transparency. Affects meshes with alpha value < 255 and cells when using peeling.
    * @param transparency
    */
    void use_transparency( bool transparency);

    bool is_using_transparency();


    /* MESH */


    /**
     * Set mesh from ovm instance without generating a new id.
     * @param mesh
     * @param instance
     */
    void update(const VMesh& mesh, OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>* instance);


    /**
     * Set mesh from file without generating a new id.
     * @param mesh
     * @param path file path
     */
    void update(const VMesh& mesh, const std::string& path);

    /**
     * Set mesh from file without generating a new id.
     * @param mesh
     * @param path file path
     */
    void update(const VMesh& mesh, const char* path);


    /**
     * Remove all meshes from volumeshOS.
     */
    void clear();

    /**
     * Remove mesh from volumeshOS.
     * @param mesh mesh to remove
     */
    void clear(const VMesh& mesh);



    /**
     * Returns the OpenVolumeMesh instance of a mesh.
     * @param mesh mesh to return
     * @return ovm instance
     */
    OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>* get_ovm(const VMesh& mesh);

    /**
     * Returns a list of all loaded meshes.
     * @return list of VMeshes
     */
    std::vector<VMesh> get_meshes();

    /**
     * Load a configuration file for a mesh. This way settings can be saved across several program executions.
     * @param mesh
     * @param path path to config file
     */
    void load_configuration(const VMesh& mesh, const std::string& path);

    /**
     * Save a configuration file for a mesh. This way settings can be saved across several program executions.
     * @param mesh
     * @param path path to config file
     */
    void save_configuration(const VMesh& mesh, const std::string& path);

    /**
     * Enable/Disable backface culling for a mesh. In special cases some faces wants to be seen from behind.
     * @param mesh
     * @param culling
     */
    void use_backface_culling(const VMesh& mesh, bool culling);

    bool is_using_backface_culling(const VMesh& mesh);

    /**
     * Enable/Disable two sided lighting for a mesh.
     * @param mesh
     * @param ts_lighting
     */
    void use_two_sided_lighting(const VMesh& mesh, bool ts_lighting);

    bool is_using_two_sided_lighting(const VMesh& mesh);

    /**
     * Enable/Disable base color for a mesh.
     * When enabled the color for the whole mesh can be specified in the GUI. Otherwise ever cell has its own color.
     * @param mesh
     * @param base_color
     */
    void use_base_color(const VMesh& mesh, bool base_color);

    bool is_using_base_color(const VMesh& mesh);

    /**
     * Set color for all meshes
     * @param color color vector in range [0,1]
     */
    template<typename Vec4T>
    void set_color(const Vec4T& color);

    /**
     * Set color for one meshes
     * @param mesh
     * @param color color vector in range [0,1]
     */
    template<typename Vec4T>
    void set_color(const VMesh& mesh, const Vec4T& color);

    template<typename Vec4T>
    Vec4T get_color(const VMesh& mesh);

    /**
     * Set color for one meshes
     * @param mesh
     * @param cell OVM Cell handle
     * @param color color vector in range [0,1]
     */
    template<typename Vec4T>
    void set_color(const VMesh& mesh, OpenVolumeMesh::CellHandle cell, const Vec4T& color);

    template<typename Vec4T>
    [[nodiscard]] Vec4T get_color(const VMesh& mesh, OpenVolumeMesh::CellHandle cell);

    /**
     * Set color for one meshes
     * @param mesh
     * @param face OVM Face handle
     * @param color color vector in range [0,1]
     */
    template<typename Vec4T>
    void set_color(const VMesh& mesh, OpenVolumeMesh::FaceHandle face, const Vec4T& color);

    /**
     * Set color for one meshes
     * @param mesh
     * @param halfface OVM Halfface handle
     * @param color color vector in range [0,1]
     */
    template<typename Vec4T>
    void set_color(const VMesh& mesh, OpenVolumeMesh::HalfFaceHandle halfface, const Vec4T& color);

    template<typename Vec4T>
    Vec4T get_color(const VMesh& mesh, OpenVolumeMesh::HalfFaceHandle halfface);

    /**
     * Select a Cell. Selected Cells gets highlighted and callbacks are called.
     * @param mesh
     * @param cell OVM Cell handle
     */
    void select(const VMesh& mesh, OpenVolumeMesh::CellHandle cell);

    /**
     * Select a Face. Selected Faces gets highlighted and callbacks are called.
     * @param mesh
     * @param face OVM Face handle
     */
    void select(const VMesh& mesh, OpenVolumeMesh::FaceHandle face);

    /**
     * Select a Halfface. Selected Halffaces gets highlighted and callbacks are called.
     * @param mesh
     * @param halfface OVM Halfface handle
     */
    void select(const VMesh& mesh, OpenVolumeMesh::HalfFaceHandle halfface);

    /**
     * Select a Edge. Selected Edges gets highlighted and callbacks are called.
     * @param mesh
     * @param edge OVM Edge handle
     */
    void select(const VMesh& mesh, OpenVolumeMesh::EdgeHandle edge);

    /**
     * Select a Vertex. Selected Vertices gets highlighted and callbacks are called.
     * @param mesh
     * @param vertex OVM Vertex handle
     */
    void select(const VMesh& mesh, OpenVolumeMesh::VertexHandle vertex);

    /**
     * Deselect a Cell.
     * @param mesh
     * @param cell OVM Cell handle
     */
    void deselect(const VMesh& mesh, OpenVolumeMesh::CellHandle cell);

    /**
     * Deselect a Face.
     * @param mesh
     * @param face OVM Face handle
     */
    void deselect(const VMesh& mesh, OpenVolumeMesh::FaceHandle face);

    /**
     * Deselect a Halfface.
     * @param mesh
     * @param halfface OVM Halfface handle
     */
    void deselect(const VMesh& mesh, OpenVolumeMesh::HalfFaceHandle halfface);

    /**
     * Deselect a Edge.
     * @param mesh
     * @param edge OVM Edge handle
     */
    void deselect(const VMesh& mesh, OpenVolumeMesh::EdgeHandle edge);

    /**
     * Deselect a Vertex.
     * @param mesh
     * @param vertex OVM Vertex handle
     */
    void deselect(const VMesh& mesh, OpenVolumeMesh::VertexHandle vertex);

    /**
     * Deselect every selected entity.
     * @param mesh
     */
    void reset_selection(const VMesh& mesh);


    /* MESH LIGHTING */

    /**
     * Set Lighting mode for a mesh.
     * @param mesh
     * @param mode
     * PHONG: Phong lighting. Affected by ambient, diffuse, specular term.
     * PBR: Physically Based Rendering. Affected by metallic and roughness term.
     */
    void set_lighting_mode(const VMesh& mesh, LightingMode mode);

    LightingMode get_lighting_mode(const VMesh& mesh);


    /**
     * Set ambient term for Phong Lighting.
     * @param mesh
     * @param ambient value in range [0,1]
     */
    void set_ambient(const VMesh& mesh, float ambient);

    float get_ambient(const VMesh& mesh);

    /**
     * Set diffuse term for Phong Lighting.
     * @param mesh
     * @param diffuse value in range [0,1]
     */
    void set_diffuse(const VMesh& mesh, float diffuse);

    float get_diffuse(const VMesh& mesh);

    /**
     * Set specular term for Phong Lighting.
     * @param mesh
     * @param specular value in range [0,1]
     */
    void set_specular(const VMesh& mesh, float specular);

    float get_specular(const VMesh& mesh);

    /**
     * Set specular coefficient for Phong Lighting.
     * @param mesh
     * @param coefficient value in range [0,10]
     */
    void set_specular_coefficient(const VMesh& mesh, float coefficient);

    float get_specular_coefficient(const VMesh& mesh);

    /**
     * Set metallic term for PBR Lighting.
     * @param mesh
     * @param metallic value in range [0,1]
     */
    void set_metallic(const VMesh& mesh, float metallic);

    float get_metallic(const VMesh& mesh);

    /**
     * Set roughness term for PBR Lighting.
     * @param mesh
     * @param roughness value in range [0,1]
     */
    void set_roughness(const VMesh& mesh, float roughness);

    float get_roughness(const VMesh& mesh);


    /**
     * Set the position of a mesh in world coordinates.
     * @param x X coordinate
     * @param y Y coordinate
     * @param z Z coordinate
     */
    void set_position(const VMesh& mesh, float x, float y, float z);

    /**
     * Set position of a mesh in world coordinates.
     * @param position coordinate vector
     */
    template<typename Vec3T>
    void set_position(const VMesh& mesh, const Vec3T& position);

    template<typename Vec3T>
    [[nodiscard]] Vec3T get_position(const VMesh& mesh);

    /**
     * Set scale of a mesh.
     * @param scale
     */
    void set_scale(const VMesh& mesh, float scale);

    float get_scale(const VMesh& mesh);

    // Set the rotation of a mesh using euler angles
    void set_rotation(const VMesh& mesh, float x, float y, float z);

    // Set the rotation of a mesh
    template<typename Vec3T>
    void set_rotation(const VMesh& mesh, const Vec3T& rotation);

    template<typename Vec3T>
    Vec3T get_rotation(const VMesh& mesh);

    template<typename Vec3T>
    Vec3T get_transformed_point(const VMesh& mesh, const Vec3T& point);

    // Resets the rotation of a mesh
    void reset_rotation(const VMesh& mesh);

    /**
     * Set scale of a mesh.
     * @param mesh
     * @param factor value in range [0,1]
     */
    void set_slice_factor(const VMesh& mesh, float factor);

    float get_slice_factor(const VMesh& mesh);


    /**
     * Lock the direction of the slice plane. Aligned by camera view direction.
     * @param mesh
     * @param lock
     */
    void set_slice_locked(const VMesh& mesh, bool lock);

    bool get_slice_locked(const VMesh& mesh);

    /**
     * Set the peel level for a given mesh.
     * @param mesh
     * @param level Value between 0 (no peel) and the total number of depth layers in the mesh.
     *              Float values can be displayed transparent when transparency is active.
     */
    void set_peel_level(const VMesh& mesh, float level);

    float get_peel_level(const VMesh& mesh);

    // Returns maximum peel depth. Depends on mesh
    /**
     * Returns maximum peel depth. Depends on mesh.
     * @param mesh
     * @return maximum peel depth
     */
    int get_max_peel_depth(const VMesh& mesh);

    /**
     * Enable/Disable reverse peeling for a mesh. Inner layers gets peeled first.
     * @param mesh
     * @param reverse
     */
    void use_reverse_peeling(const VMesh& mesh, bool reverse);

    bool is_using_reverse_peeling(const VMesh& mesh);

    /**
     * Set the rounding factor for each cell of a mesh.
     * @param mesh
     * @param rounding 0 (no rounding) to 1 (full rounding).
     */
    void set_cell_rounding(const VMesh& mesh, float rounding);

    float get_cell_rounding(const VMesh& mesh);

    // Set the tessellation level for Bézier meshes (non-Bézier meshes are not affected)
    void set_tessellation_level(const VMesh& mesh, int level);

    // Return the tessellation level of a given mesh. (0 for non-Bézier meshes)
    int get_tessellation_level(const VMesh& mesh);


    /**
     * Set the cell size of a given mesh.
     * @param mesh
     * @param rounding 0 (infinitely small) to 1 (original size).
     */
    void set_cell_size(const VMesh& mesh, float size);

    float get_cell_size(const VMesh& mesh);

    /**
     * Set Cell (in-)visible.
     * @param mesh
     * @param cell OVM Cell handle
     * @param visible
     */
    void set_visibility(const VMesh& mesh, OpenVolumeMesh::CellHandle cell, bool visible);

    bool get_visibility(const VMesh& mesh, OpenVolumeMesh::CellHandle cell);

    /**
     * Set mesh (in-)visible.
     * @param mesh
     * @param visible
     */
    void set_visibility(const VMesh& mesh, bool visible);

    bool get_visibility(const VMesh& mesh);

    /**
     * Set all cells visible.
     * @param mesh
     */
    void reset_visibility(const VMesh& mesh);

    /**
     * Isolate a single cell of a mesh, making it the only visible cell.
     * @param mesh
     * @param cell OVM Cell handle
     */
    void isolate(const VMesh& mesh, OpenVolumeMesh::CellHandle cell);

    /**
     * Dig a single cell of a mesh, making it invisible.
     * @param mesh
     * @param cell OVM Cell handle
     */
    void dig(const VMesh& mesh, OpenVolumeMesh::CellHandle cell);

    /**
     * Returns true if the mesh is a valid handle.
     * @param mesh
     * @return validity of VMesh
     */
    [[nodiscard]] bool is_valid(const VMesh& mesh);

    // Returns true if the mesh is a Bézier mesh
    [[nodiscard]] bool is_bezier_mesh(const VMesh& mesh);


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


    // Set Lighting mode
    void set_shape_lighting_mode(LightingMode mode);

    LightingMode get_shape_lighting_mode();

    // Set the ambient term for the phong lighting model of all shapes
    void set_shape_ambient(float ambient);

    float get_shape_ambient();

    // Set the diffuse term for the phong lighting model of all shapes
    void set_shape_diffuse(float diffuse);

    float get_shape_diffuse();

    // Set the specular term for the phong lighting model of all shapes
    void set_shape_specular(float specular);

    float get_shape_specular();

    // Set the specular coefficient for the phong lighting model of all shapes
    void set_shape_specular_coefficient(float coefficient);

    float get_shape_specular_coefficient();

    // Set the metallic term for the pbr lighting model of all shapes
    void set_shape_metallic(float metallic);

    float get_shape_metallic();

    // Set the roughness term for the pbr lighting model of all shapes
    void set_shape_roughness(float roughness);

    float get_shape_roughness();


    /* Miscellaneous */

    struct ExportOptions
    {
        int width               = -1;       // viewport width by default
        int height              = -1;       // viewport height by default
        bool include_background = true;     // include background in image
        bool include_shapes     = true;     // include shapes in image
        bool include_ground     = true;     // include ground in image
        bool ground_shadow_only = false;    // if ground and shadows are active, export only the area in shadow
    };

    // Export the current viewport as png
    void export_image(const ExportOptions& options = {});

    // Export the current viewport as png to a specified file
    void export_image(const std::string& path, const ExportOptions& options = {});


    /* WINDOW */

    /**
     * Determines if input events should be registered and handled
     * @param block
     */
    void block_inputs(bool block);

    bool is_input_blocked();

    /**
     * Open a file dialog with a given title.
     * @param title title for file dialog
     * @return file path or nullptr if path does not exist
     */
    const char* file_dialog(const std::string& title);

    /**
     * Set the color theme of volumeshOS
     * @param theme
     * Light: Lightmode
     * Dark:  Darkmode
     */
    void set_theme(Theme theme);

    /**
     * Returns the current width of the viewport
     * @return
     */
    int get_viewport_width();

    /**
     * Returns the current height of the viewport
     * @return
     */    int get_viewport_height();


    /* LOG_WINDOW */

    /**
     * Send log message to Logwindow
     * @param message
     */
    void log(const std::string& message);

    /**
     * Send warn message to Logwindow
     * @param message
     */
    void warn(const std::string& message);

    /**
     * Send error message to Logwindow
     * @param message
     */
    void error(const std::string& message);

    /**
     * Clear logs.
     */
    void clear_logs();

    /**
     * Enable/Disable Logwindow
     * @param log_window
     */
    void use_log_window(bool log_window);


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

        inline void use_backface_culling(const bool culling) const
        {
            volumeshOS::use_backface_culling(*this, culling);
        }

        [[nodiscard]] inline bool is_using_backface_culling() const
        {
            return volumeshOS::is_using_backface_culling(*this);
        }

        [[nodiscard]] inline bool is_using_two_sided_lighting() const
        {
            return volumeshOS::is_using_two_sided_lighting(*this);
        }

        inline void use_two_sided_lighting(const bool ts_lighting) const
        {
            volumeshOS::use_two_sided_lighting(*this, ts_lighting);
        }

        [[nodiscard]] inline bool is_using_base_color() const
        {
            return volumeshOS::is_using_base_color(*this);
        }

        inline void use_base_color(bool base_color)const
        {
            volumeshOS::use_base_color(*this, base_color);
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

        // Returns the ambient term for the phong lighting model of a mesh
        [[nodiscard]] inline float get_ambient() const
        {
            return volumeshOS::get_ambient(*this);
        }

        // Set the diffuse term for the phong lighting model of a mesh
        inline void set_diffuse(float diffuse) const
        {
            volumeshOS::set_diffuse(*this, diffuse);
        }

        // Returns the diffuse term for the phong lighting model of a mesh
        [[nodiscard]] inline float get_diffuse() const
        {
            return volumeshOS::get_diffuse(*this);
        }

        // Set the specular term for the phong lighting model of a mesh
        inline void set_specular(float specular) const
        {
            volumeshOS::set_specular(*this, specular);
        }

        // Returns the specular term for the phong lighting model of a mesh
        [[nodiscard]] inline float get_specular() const
        {
            return volumeshOS::get_specular(*this);
        }

        // Set the specular coefficient for the phong lighting model of a mesh
        inline void set_specular_coefficient(float coefficient) const
        {
            volumeshOS::set_specular_coefficient(*this, coefficient);
        }

        // Returns the specular coefficient for the phong lighting model of a mesh
        [[nodiscard]] inline float get_specular_coefficient() const
        {
            return volumeshOS::get_specular_coefficient(*this);
        }

        // Set parameters for the phong lighting model of a mesh
        inline void set_lighting_mode(LightingMode mode) const
        {
            volumeshOS::set_lighting_mode(*this, mode);
        }

        [[nodiscard]] inline LightingMode get_lighting_mode() const
        {
            return volumeshOS::get_lighting_mode(*this);
        }

        inline void set_metallic(float metallic) const
        {
            volumeshOS::set_metallic(*this, metallic);
        }

        [[nodiscard]] inline float get_metallic() const
        {
            return volumeshOS::get_metallic(*this);
        }

        inline void set_roughness(float roughness) const
        {
            volumeshOS::set_roughness(*this, roughness);
        }

        // Set the position of a mesh
        inline void set_position(float x, float y, float z) const
        {
            volumeshOS::set_position(*this, x, y, z);
        }

        [[nodiscard]] inline float get_roughness() const
        {
            return volumeshOS::get_roughness(*this);
        }

        // Set the position of a mesh
        template<typename Vec3T>
        inline void set_position(const Vec3T& position) const
        {
            volumeshOS::set_position<Vec3T>(*this, position);
        }

        // Returns the position of a mesh
        template<typename Vec3T>
        [[nodiscard]] inline Vec3T get_position() const
        {
            return volumeshOS::get_position<Vec3T>(*this);
        }

        // Set the scale of a mesh
        inline void set_scale(float scale) const
        {
            volumeshOS::set_scale(*this, scale);
        }

        // Returns the scale of a mesh
        [[nodiscard]] inline float get_scale() const
        {
            return volumeshOS::get_scale(*this);
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

        // Returns the rotation of a mesh using euler angles
        template<typename Vec3T>
        [[nodiscard]] inline Vec3T get_rotation() const
        {
            return volumeshOS::get_rotation<Vec3T>(*this);
        }

        // Returns the rotation of a mesh using euler angles
        template<typename Vec3T>
        [[nodiscard]] inline Vec3T get_transformed_point(const Vec3T& point) const
        {
            return volumeshOS::get_transformed_point<Vec3T>(*this, point);
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

        // Returns the slice factor for a mesh. 0 (no slicing) to 1 (full slicing of the mesh)
        [[nodiscard]] inline float get_slice_factor() const
        {
            return volumeshOS::get_slice_factor(*this);
        }

        // Lock the direction of the slice plane
        inline void set_slice_lock(bool lock) const
        {
            volumeshOS::set_slice_locked(*this, lock);
        }

        // Returns the lock direction of the slice plane
        [[nodiscard]] inline bool get_slice_lock() const
        {
            return volumeshOS::get_slice_locked(*this);
        }

        // Set the peel level for a given mesh. 0 (no peel) up to the total number of depth layers in the mesh
        inline void set_peel_level(float level) const
        {
            volumeshOS::set_peel_level(*this, level);
        }

        // Returns the peel level for a given mesh. 0 (no peel) up to the total number of depth layers in the mesh
        [[nodiscard]] inline float get_peel_level() const
        {
            return volumeshOS::get_peel_level(*this);
        }

        // Set the peel level for a given mesh. 0 (no peel) up to the total number of depth layers in the mesh
        inline void set_reverse_peeling(bool reverse) const
        {
            volumeshOS::use_reverse_peeling(*this, reverse);
        }

        // Returns if reverse peeling is active
        [[nodiscard]] inline bool get_reverse_peeling() const
        {
            return volumeshOS::is_using_reverse_peeling(*this);
        }

        // Returns the number of cell depth layers of the mesh
        [[nodiscard]] inline int get_max_peel_depth() const
        {
            return volumeshOS::get_max_peel_depth(*this);
        }


        // Set the rounding factor for each cell of a mesh. 0 (no rounding) to 1 (full rounding)
        inline void set_cell_rounding(float rounding) const
        {
            volumeshOS::set_cell_rounding(*this, rounding);
        }

        // Returns the rounding factor for each cell of a mesh. 0 (no rounding) to 1 (full rounding)
        [[nodiscard]] inline float get_cell_rounding() const
        {
            return volumeshOS::get_cell_rounding(*this);
        }

        // Set the cell size of a given mesh. 0 (infinitely small) to 1 (original size)
        inline void set_cell_size(float size) const
        {
            volumeshOS::set_cell_size(*this, size);
        }

        // Returns the tessellation level of a given mesh (0 for non-Bézier meshes)
        [[nodiscard]] inline int get_tessellation_level() const
        {
            return volumeshOS::get_tessellation_level(*this);
        }

        // Set the tessellation level of a given mesh. (non-Bézier meshes are not affected)
        inline void set_tessellation_level(int level) const
        {
            volumeshOS::set_tessellation_level(*this, level);
        }

        // Returns the cell size of a given mesh. 0 (infinitely small) to 1 (original size)
        [[nodiscard]] inline float get_cell_size() const
        {
            return volumeshOS::get_cell_size(*this);
        }

        // Set currently active mesh
        inline void set_focused_mesh() const
        {
            volumeshOS::set_focused_mesh(*this);
        }

        // Set the cell visibility of a mesh
        inline void set_visibility(OpenVolumeMesh::CellHandle cell, bool visible) const
        {
            volumeshOS::set_visibility(*this, cell, visible);
        }

        // Returns the visibility of a cell
        [[nodiscard]] inline bool get_visibility(OpenVolumeMesh::CellHandle cell) const
        {
            return volumeshOS::get_visibility(*this, cell);
        }

        // Set the visibility of a mesh
        inline void set_visibility(bool visible) const
        {
            volumeshOS::set_visibility(*this, visible);
        }

        // Returns the visibility of the mesh
        [[nodiscard]] inline bool get_visibility() const
        {
            return volumeshOS::get_visibility(*this);
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
        inline void dig(OpenVolumeMesh::CellHandle cell) const
        {
            volumeshOS::dig(*this, cell);
        }

        // Set the name of a mesh
        inline void set_name(const std::string& name) const
        {
            volumeshOS::set_name(*this, name);
        }

        // Returns the name of the mesh
        [[nodiscard]] inline const std::string& get_name() const
        {
            return volumeshOS::get_name(*this);
        }


        template<typename ShapeType>
        [[nodiscard]] inline ShapeType add_shape() const
        {
            static_assert(std::is_base_of_v<VShape, ShapeType>);
            return volumeshOS::add_shape<ShapeType>(*this);
        }

        template<typename ShapeType>
        [[nodiscard]] inline ShapeType add_shape(OpenVolumeMesh::CellHandle cell) const
        {
            static_assert(std::is_base_of_v<VShape, ShapeType>);
            return volumeshOS::add_shape<ShapeType>(*this, cell);
        }


        // Returns true if the mesh is a valid handle
        [[nodiscard]] inline bool is_valid() const
        {
            return volumeshOS::is_valid(*this);
        }

        // Return true if the mesh is bezier mesh
        [[nodiscard]] inline bool is_bezier_mesh() const
        {
            return volumeshOS::is_bezier_mesh(*this);
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
