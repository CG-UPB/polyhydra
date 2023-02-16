#pragma once

#include "vospch.h"
#include "util/Enums.h"

namespace volumeshOS
{
    struct VMesh;
    struct VShape;
    struct VArrow;

    /**
     *
     */
    void init();

    /**
     * Open volumeshOS context and start rendering
     */
    void open();

    /**
     * Close volumeshOS context
     */
    void close();


    /* SETTINGS */

    /* Callbacks */

    /**
     * Define programmer user interface. This function is calles once per frame and can be used to define ImGUI Interface.
     * @param callback
     */
    void on_gui_render(const std::function<void()>& callback);

    /**
     * Convert Mesh from OpenVolumeMesh into a new VMesh object
     * @tparam KernelType OVM Kernel
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
    void set_focused_mesh(const VMesh& mesh);

    /**
     * Get given mesh into focus. Some operations only occur on the focused mesh.
     * @return
     */
    VMesh get_focused_mesh();

    /**
     * Set name for mesh
     * @param mesh VMesh
     * @param name name string
     */
    void set_name(const VMesh& mesh, const std::string& name);

    /**
     * Get name of mesh
     * @param mesh
     * @return
     */
    const std::string& get_name(const VMesh& mesh);


    /* GENERAL */

    /**
     * Set Rendering Mode.
     * @param mesh
     * @param mode
     * Points: Rendering vertices aka points \n
     * Lines: Rendering edges aka wireframe \n
     * Cells : Rendering cells (standard) \n
     */
    void set_rendering_mode(const VMesh& mesh, RenderingMode mode);

    /**
    * Get Rendering Mode.
     * @param mesh
     * @return
     * Points: Rendering vertices aka points \n
     * Lines: Rendering edges aka wireframe \n
     * Cells : Rendering cells (standard) \n
     */
    RenderingMode get_rendering_mode(const VMesh& mesh);

    /**
     * Set Point Size. Determines the size of rendered points when using ::RenderingMode = Points.
     * @param mesh
     * @param size point size
     */
    void set_point_size(const VMesh& mesh, float size);

    /**
     * Get Point Size.
     * @param mesh
     */
    float get_point_size(const VMesh& mesh);


    /**
     * Set Line Width. Determines the width of the rendered lines when using ::RenderingMode = Lines.
     * @param mesh
     * @param width line width
     */
    void set_line_width(const VMesh& mesh, float width);

    /**
     * Get Line Width.
     * @param mesh
     */
    float get_line_width(const VMesh& mesh);

    /**
     * Set Shading Mode.
     * @param mesh
     * @param mode
     * Flat: Rendering mesh using flat shading \n
     * Phong: Rendering mesh using phong shading \n
     */
    void set_shading_mode(const VMesh& mesh, ShadingMode mode);

    /**
     * Get Shading Mode.
     * @param mesh
     * @return
     * Flat: Rendering mesh using flat shading \n
     * Phong: Rendering mesh using phong shading \n
     */
    ShadingMode get_shading_mode(const VMesh& mesh);

    /**
     * Set Sky color
     * @tparam Vec3T 3D vector type
     * @param color vector in range [0,1]
     */
    template<typename Vec3T>
    void set_sky_color(const Vec3T& color);

    /**
     * Get Sky color
     * @tparam Vec3T
     * @return color vector in range [0,1]
     */
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

    /**
     * Get Selection Mode.
     * OFF: Selection off
     * VERTEX: Select Vertices
     * EDGE: Select Edges
     * FACE: Select Faces
     * ALL: Select everything at once
     * @return
     */
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

    /**
     *
     * @return
     */
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
      * @tparam Vec3T 3D vector type
      * @param position coordinate vector
      */
    template<typename Vec3T>
    void set_camera_position(const Vec3T& position);

    /**
     * Get camera postion in world coordinates.
     * @tparam Vec3T 3D vector type
     * @return
     */
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
      * @tparam Vec3T
      * @param target coordinate vector
      */
    template<typename Vec3T>
    void set_camera_target(const Vec3T& target);

    /**
     * Get camera target in world coordinates.
     * @tparam Vec3T 3D vector type
     * @return
     */
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

    /**
     * Get camera filed of view
     * @return value between 1 and 90 (degree).
     */
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
     * @param gamma value in range [0,1]
     */
    void set_gamma(float gamma);

    /**
     * Get gamma value for post processing.
     * @return value in range [0,1]
     */
    float get_gamma();

    /**
     * Set saturation value for post processing.
     * @param saturation value in range [0,1]
     */
    void set_saturation(float saturation);

    /**
     * Get saturation value for post processing.
     * @return saturation value in range [0,1]
     */
    float get_saturation();

    /**
     * Set contrast value for post processing.
     * @param contrast value in range [0,1]
     */
    void set_contrast(float contrast);

    /**
     * Get contrast value for post processing.
     * @return value in range [0,1]
     */
    float get_contrast();


    /* GROUND */

    /**
     * Enable/Disable ground. The ground receives shadow.
     * @param ground
     */
    void use_ground(bool ground);

    /**
     * True when ground is enabled.
     * @return is_using_ground
     */
    bool is_using_ground();

    /**
     * Enable/Disable grid. The grid can lay on top of the ground.
     * @param ground
     */
    void use_grid(bool grid);

    /**
     * True when grid is enabled.
     * @return is_using_grid
     */
    bool is_using_grid();

    /**
     * Set ground color.
     * @tparam Vec3T 3D vector type
     * @param color vector in range [0,1]
     */
    template<typename Vec3T>
    void set_ground_color(const Vec3T& color);

    /**
     * Get ground color vector.
     * @tparam Vec3T 3D vector type
     * @return color vector in range [0,1]
     */
    template<typename Vec3T>
    Vec3T get_ground_color();

    /**
     * Set grid color.
     * @param color vector in range [0,1]
     */
    template<typename Vec3T>
    void set_grid_color(const Vec3T& color);

    /**
     * Get grid color vector.
     * @tparam Vec3T 3D vector type
     * @return color vector in range [0,1]
     */
    template<typename Vec3T>
    Vec3T get_grid_color();

    /**
     * Set ground height in world coordinates.
     * @param height value
     */
    void set_ground_height(float height);

    /**
     * Get ground height.
     * @return value
     */
    float get_gound_height();


    /* SHADOWS */

    /**
     * Enable/Disable shadows.
     * @param shadows
     */
    void use_shadows(bool shadows);

    /**
     * True if shadows are enabled
     * @return is_using_shadows
     */
    bool is_using_shadows();


    /* AMBIENT OCCLUSION */

    /**
     * Enable/Disable ambient occlusion.
     * @param shadows
     */
    void use_ambient_occlusion( bool ssao);

    /**
     * True if ambient occlusion is enabled.
     * @return
     */
    bool is_using_ambient_occlusion();

    /**
    * Enable/Disable transparency. Affects meshes with alpha value < 255 and cells when using peeling.
    * @param transparency
    */
    void use_transparency( bool transparency);

    /**
     * True if transparency is enabled.
     * @return is_using_transparency
     */
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

    /**
     * True if backface culling is enabled for mesh
     * @param mesh
     * @return is_using_backface_culling
     */
    bool is_using_backface_culling(const VMesh& mesh);

    /**
     * Enable/Disable two sided lighting for a mesh.
     * @param mesh
     * @param ts_lighting
     */
    void use_two_sided_lighting(const VMesh& mesh, bool ts_lighting);

    /**
     * True if two sided lighting is enabled for mesh
     * @param mesh
     * @return is_using_two_sided_lighting
     */
    bool is_using_two_sided_lighting(const VMesh& mesh);

    /**
     * Enable/Disable base color for a mesh.
     * When enabled the color for the whole mesh can be specified in the GUI. Otherwise ever cell has its own color.
     * @param mesh
     * @param base_color
     */
    void use_base_color(const VMesh& mesh, bool base_color);

    /**
     * True if base color is used for mesh
     * @param mesh
     * @return is_using_base_color
     */
    bool is_using_base_color(const VMesh& mesh);

    /**
     * Set color for all meshes
     * @tparam Vec4T 4D vector type
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

    /**
     * Get color of mesh
     * @tparam Vec4T 4D vector type
     * @param mesh
     * @return color vector in range [0,1]
     */
    template<typename Vec4T>
    Vec4T get_color(const VMesh& mesh);

    /**
     * Set color for one meshes
     * @param mesh
     * @param cell OVM Cell handle
     * @param color vector in range [0,1]
     */
    template<typename Vec4T>
    void set_color(const VMesh& mesh, OpenVolumeMesh::CellHandle cell, const Vec4T& color);

    /**
     * Get color of a cell
     * @tparam Vec4T 4D vector type
     * @param mesh
     * @param cell OVM Cell handle
     * @return color vector in range [0,1]
     */
    template<typename Vec4T>
    [[nodiscard]] Vec4T get_color(const VMesh& mesh, OpenVolumeMesh::CellHandle cell);

    /**
     * Set color for one meshes
     * @tparam Vec4T 4D vector type
     * @param mesh
     * @param face OVM Face handle
     * @param color color vector in range [0,1]
     */
    template<typename Vec4T>
    void set_color(const VMesh& mesh, OpenVolumeMesh::FaceHandle face, const Vec4T& color);

    /**
     * Set color for one meshes
     * @tparam Vec4T 4D vector type
     * @param mesh
     * @param halfface OVM Halfface handle
     * @param color vector in range [0,1]
     */
    template<typename Vec4T>
    void set_color(const VMesh& mesh, OpenVolumeMesh::HalfFaceHandle halfface, const Vec4T& color);

    /**
     * Get color of a halfface
     * @tparam Vec4T 4D vector type
     * @param mesh
     * @param halfface
     * @return color vector in range [0,1]
     */
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

    /**
     * Get Lighting mode of a mesh
     * @param mesh
     * PHONG: Phong lighting. Affected by ambient, diffuse, specular term.
     * PBR: Physically Based Rendering. Affected by metallic and roughness term.
     * @return
     */
    LightingMode get_lighting_mode(const VMesh& mesh);


    /**
     * Set ambient term for Phong Lighting.
     * @param mesh
     * @param ambient value in range [0,1]
     */
    void set_ambient(const VMesh& mesh, float ambient);

    /**
     * Get ambient term for Phong Lighting.
     * @param mesh
     * @return ambient value in range [0,1]
     */
    float get_ambient(const VMesh& mesh);

    /**
     * Set diffuse term for Phong Lighting.
     * @param mesh
     * @param diffuse value in range [0,1]
     */
    void set_diffuse(const VMesh& mesh, float diffuse);

    /**
     * Get diffuse term for Phong Lighting.
     * @param mesh
     * @return diffuse value in range [0,1]
     */
    float get_diffuse(const VMesh& mesh);

    /**
     * Set specular term for Phong Lighting.
     * @param mesh
     * @param specular value in range [0,1]
     */
    void set_specular(const VMesh& mesh, float specular);

    /**
     * Get specular term for Phong Lighting.
     * @param mesh
     * @return specular value in range [0,1]
     */
    float get_specular(const VMesh& mesh);

    /**
     * Set specular coefficient for Phong Lighting.
     * @param mesh
     * @param coefficient value in range [0,10]
     */
    void set_specular_coefficient(const VMesh& mesh, float coefficient);

    /**
     * Get specular coefficient for Phong Lighting.
     * @param mesh
     * @return coefficient value in range [0,10]
     */
    float get_specular_coefficient(const VMesh& mesh);

    /**
     * Set metallic term for PBR Lighting.
     * @param mesh
     * @param metallic value in range [0,1]
     */
    void set_metallic(const VMesh& mesh, float metallic);

    /**
     * Get metallic term for PBR Lighting.
     * @param mesh
     * @return metallic value in range [0,1]
     */
    float get_metallic(const VMesh& mesh);

    /**
     * Set roughness term for PBR Lighting.
     * @param mesh
     * @param roughness value in range [0,1]
     */
    void set_roughness(const VMesh& mesh, float roughness);

    /**
     * Get roughness term for PBR Lighting.
     * @param mesh
     * @return roughness value in range [0,1]
     */
    float get_roughness(const VMesh& mesh);


    /**
     * Set the position of a mesh in world coordinates.
     * @param mesh
     * @param x X coordinate
     * @param y Y coordinate
     * @param z Z coordinate
     */
    void set_position(const VMesh& mesh, float x, float y, float z);

     /**
      * Set position of a mesh in world coordinates.
      * @tparam Vec3T 3D vector type
      * @param mesh
      * @param position coordinate vector
      */
    template<typename Vec3T>
    void set_position(const VMesh& mesh, const Vec3T& position);

    /**
     * Get position of a mesh in world coordinates.
     * @tparam Vec3T 3D vector type
     * @param mesh
     * @return position coordinate vector
     */
    template<typename Vec3T>
    [[nodiscard]] Vec3T get_position(const VMesh& mesh);

    /**
     * Set scale of a mesh.
     * @param mesh
     * @param scale
     */
    void set_scale(const VMesh& mesh, float scale);

    /**
     * Get scale of mesh
     * @param mesh
     * @return scale
     */
    float get_scale(const VMesh& mesh);

    /**
     * Set the rotation of a mesh using euler angles
     * @param mesh
     * @param x
     * @param y
     * @param z
     */
    void set_rotation(const VMesh& mesh, float x, float y, float z);

    /**
     * Set the rotation of a mesh.
     * @tparam Vec3T 3D vector type
     * @param mesh
     * @param rotation vector in range [0,1]
     */
    template<typename Vec3T>
    void set_rotation(const VMesh& mesh, const Vec3T& rotation);

    /**
     * Get the rotation of a mesh.
     * @tparam Vec3T 3D vector type
     * @param mesh
     * @return rotation vector in range [0,1]
     */
    template<typename Vec3T>
    Vec3T get_rotation(const VMesh& mesh);

    /**
     * Get point transform
     * @tparam Vec3T 3D vector type
     * @param mesh
     * @param point
     * @return
     */
    template<typename Vec3T>
    Vec3T get_transformed_point(const VMesh& mesh, const Vec3T& point);

    /**
     * Resets the rotation of a mesh.
     * @param mesh
     */
    void reset_rotation(const VMesh& mesh);

    /**
     * Set scale of a mesh.
     * @param mesh
     * @param factor value in range [0,1]
     */
    void set_slice_factor(const VMesh& mesh, float factor);

    /**
     * Get slice factor
     * @param mesh
     * @return value in range [0,1]
     */
    float get_slice_factor(const VMesh& mesh);


    /**
     * Lock the direction of the slice plane. Aligned by camera view direction.
     * @param mesh
     * @param lock
     */
    void set_slice_locked(const VMesh& mesh, bool lock);

    /**
     * True when slicing is locked.
     * @param mesh
     * @return slice_locked
     */
    bool get_slice_locked(const VMesh& mesh);

    /**
     * Set the peel level for a given mesh.
     * @param mesh
     * @param level value between 0 (no peel) and the total number of depth layers in the mesh.
     *              Float values can be displayed transparent when transparency is active.
     */
    void set_peel_level(const VMesh& mesh, float level);

    /**
     * Get peel level of a mesh
     * @param mesh
     * @return value between 0 (no peel) and the total number of depth layers in the mesh.
     */
    float get_peel_level(const VMesh& mesh);

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

    /**
     * True when reverse_peeling is enabled.
     * @param mesh
     * @return is_using_reverse_peeling
     */
    bool is_using_reverse_peeling(const VMesh& mesh);

    /**
     * Set the rounding factor for each cell of a mesh.
     * @param mesh
     * @param rounding 0 (no rounding) to 1 (full rounding).
     */
    void set_cell_rounding(const VMesh& mesh, float rounding);

    /**
     * Get the rounding factor of a mesh.
     * @param mesh
     * @return rounding 0 (no rounding) to 1 (full rounding).
     */
    float get_cell_rounding(const VMesh& mesh);

    /**
     * Set the tessellation level for Bézier meshes (non-Bézier meshes are not affected)
     * @param mesh
     * @param level value in range [0,64]
     */
    void set_tessellation_level(const VMesh& mesh, int level);

    /**
     * Get the tessellation level for Bézier meshes (non-Bézier meshes are not affected)
     * @param mesh
     * @return level value in range [0,64]
     */
    int get_tessellation_level(const VMesh& mesh);


    /**
     * Set the cell size of a given mesh.
     * @param mesh
     * @param rounding 0 (infinitely small) to 1 (original size).
     */
    void set_cell_size(const VMesh& mesh, float size);

    /**
     * Get the cell size of a given mesh.
     * @param mesh
     * @return rounding 0 (infinitely small) to 1 (original size)
     */
    float get_cell_size(const VMesh& mesh);

    /**
     * Set Cell (in-)visibility.
     * @param mesh
     * @param cell OVM Cell handle
     * @param visible
     */
    void set_visibility(const VMesh& mesh, OpenVolumeMesh::CellHandle cell, bool visible);

    /**
     * Get cell (in-)visibility
     * @param mesh
     * @param cell
     * @return visible
     */
    bool get_visibility(const VMesh& mesh, OpenVolumeMesh::CellHandle cell);

    /**
     * Set mesh (in-)visible.
     * @param mesh
     * @param visible
     */
    void set_visibility(const VMesh& mesh, bool visible);

    /**
     * Get mesh (in-)visible.
     * @param mesh
     * @return visible
     */
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

    /**
     * True if the mesh is a Bézier mesh
     * @param mesh
     * @return is_bezier
     */
    [[nodiscard]] bool is_bezier_mesh(const VMesh& mesh);


    /* Shapes */

    /**
     * Add shape
     * @tparam ShapeType VArrow,VBox,VSphere,VCylinder,VCone
     * @return VShape object
     */
    template<typename ShapeType>
    [[nodiscard]] ShapeType add_shape();

    /**
     * Add shape to mesh
     * @tparam ShapeType VArrow,VBox,VSphere,VCylinder,VCone
     * @param mesh
     * @return VShape object
     */
    template<typename ShapeType>
    [[nodiscard]] ShapeType add_shape(const VMesh& mesh);

    /**
     * Add shape to cell. This way they share properties with it (like visibility and size).
     * @tparam ShapeType VArrow,VBox,VSphere,VCylinder,VCone
     * @param mesh
     * @param cell OVM cell handle
     * @return VShape object
     */
    template<typename ShapeType>
    [[nodiscard]] ShapeType add_shape(const VMesh& mesh, OpenVolumeMesh::CellHandle cell);

    /**
     * Remove shape.
     * @param shape
     */
    void remove_shape(const VShape& shape);

    /**
     * Remove all shapes.
     */
    void remove_shapes();

    /**
     * Set the position of this shape in
     * @param shape
     * @param x X coordinate
     * @param y Y coordinate
     * @param z Z coordinate
     */
    void set_position(const VShape& shape, float x, float y, float z);

    /**
     * Set the position of this shape
     * @tparam Vec3T 3D vector type
     * @param shape
     * @param position vector
     */
    template<typename Vec3T>
    void set_position(const VShape& shape, const Vec3T& position);

    /**
     * Set the orientation of this shape using an axis and an angle. The axis is up (0, 1, 0) by default
     * @param shape
     * @param axis_x X component of the rotation axis
     * @param axis_y Y component of the rotation axis
     * @param axis_z Z component of the rotation axis
     * @param angle rotation angle around the given axis in radians
     */
    void set_direction(const VShape& shape, float axis_x, float axis_y, float axis_z, float angle = 0.0f);

    /**
     * Set the orientation of this shape using an axis and an angle. The axis is up (0, 1, 0) by default
     * @param shape
     * @tparam Vec3T 3D Vector type
     * @param axis rotation axis
     * @param angle rotation angle around the given axis in radians
     */
    template<typename Vec3T>
    void set_direction(const VShape& shape, const Vec3T& axis, float angle = 0.0f);

    /**
     * Set the scale of this shape for all axis. Default is (1, 1, 1)
     * @param shape
     * @param scalar scale value
     */
    void set_scale(const VShape& shape, float scalar);

    /**
     * Set the scale of this shape by axis. Default is (1, 1, 1)
     * @param shape
     * @param x X scale
     * @param y Y scale
     * @param z Z scale
     */
    void set_scale(const VShape& shape, float x, float y, float z);

    /**
     * Set the scale of this shape by axis. Default is (1, 1, 1)
     * @param shape
     * @tparam Vec3T 3D Vector type
     * @param scale scale vector
     */
    template<typename Vec3T>
    void set_scale(const VShape& shape, const Vec3T& scale);

    /**
     * Set the color of this shape. The default color is white (1, 1, 1, 1)
     * @param shape
     * @tparam Vec4T 4D Vector type
     * @param color vector containing the rgba components
     */
    template<typename Vec4T>
    void set_color(const VShape& shape, const Vec4T& color);

    /**
     * Returns the current position of this shape
     * @param shape
     * @tparam Vec3T 3D Vector type
     * @return current position
     */
    template<typename Vec3T>
    [[nodiscard]] Vec3T get_position(const VShape& shape);

    /**
     * Returns the current scale of this shape
     * @param shape
     * @tparam Vec3T 3D Vector type
     * @return current scale
     */
    template<typename Vec3T>
    [[nodiscard]] Vec3T get_scale(const VShape& shape);

    /**
     * Set the start of the arrow tip in percentage (between 0 and 1). So a height of 0.7 means 30% tip and 70% base
     * @param shape VArrow object
     * @param tip_height height of the arrow tip in percentage
     */
    void set_tip_height(const VArrow& shape, float tip_height);

    /**
     * Set the width of the arrow base in percentage (default 0.5). So a width of 0.3 means 30% of the tip width
     * @param shape VArrow object
     * @param base_width width of the arrow base in percentage
     */
    void set_base_width(const VArrow& shape, float base_width);


    // Set Lighting mode
    void set_shape_lighting_mode(LightingMode mode);

    LightingMode get_shape_lighting_mode();

    /**
     * Set the ambient term for the phong lighting model of all shapes
     * @param ambient value in range [0,1]
     */
    void set_shape_ambient(float ambient);

    /**
     * Get the ambient term for the phong lighting model of all shapes
     * @return ambient value in range [0,1]
     */
    float get_shape_ambient();

    /**
     * Set the diffuse term for the phong lighting model of all shapes
     * @param diffuse value in range [0,1]
     */
    void set_shape_diffuse(float diffuse);

    /**
     * Get the diffuse term for the phong lighting model of all shapes
     * @return diffuse value in range [0,1]
     */
    float get_shape_diffuse();

    /**
     * Set the specular term for the phong lighting model of all shapes
     * @param specular value in range [0,1]
     */
    void set_shape_specular(float specular);

    /**
     * Get the specular term for the phong lighting model of all shapes
     * @return specular value in range [0,1]
     */
    float get_shape_specular();

    /**
     * Set the specular coefficient for the phong lighting model of all shapes
     * @param coefficient value in range [0,10]
     */
    void set_shape_specular_coefficient(float coefficient);

    /**
     * Get the specular coefficient for the phong lighting model of all shapes
     * @return coefficient value in range [0,10]
     */
    float get_shape_specular_coefficient();

    /**
     * Set the metallic term for the pbr lighting model of all shapes
     * @param metallic value in range [0,1]
     */
    void set_shape_metallic(float metallic);

    /**
     * Get the metallic term for the pbr lighting model of all shapes
     * @return metallic value in range [0,1]
     */
    float get_shape_metallic();

    /**
     * Set the roughness term for the pbr lighting model of all shapes
     * @param roughness value in range [0,1]
     */
    void set_shape_roughness(float roughness);

    /**
     * Get the roughness term for the pbr lighting model of all shapes
     * @return roughness value in range [0,1]
     */
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

    /**
     * Export the current viewport as png
     * @param options
     */
    void export_image(const ExportOptions& options = {});

    /**
     * Export the current viewport as png to a specified file
     * @param path
     * @param options
     */
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
     */
    int get_viewport_height();


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

    /**
     * Utility wrapper class for using shapes
     */
    struct VShape
    {
        /**
         * This should not be called to construct a shape, use the api-method to add shapes
         * @param id internal shape id
         */
        explicit VShape(int id = -1) : m_id(id)
        {}

        /**
         * Remove this shape from the scene
         */
        inline void remove() const
        {
            volumeshOS::remove_shape(*this);
        }

        /**
         * Set the position of this shape
         *
         * @param x X coordinate
         * @param y Y coordinate
         * @param z Z coordinate
         */
        inline void set_position(float x, float y, float z) const
        {
            volumeshOS::set_position(*this, x, y, z);
        }

        /**
         * Set the position of this shape
         *
         * @tparam Vec3T 3D vector type
         * @param position position to be set
         */
        template<typename Vec3T>
        inline void set_position(const Vec3T& position) const
        {
            volumeshOS::set_position<Vec3T>(*this, position);
        }

        /**
         * Set the orientation of this shape using an axis and an angle. The axis is up (0, 1, 0) by default
         *
         * @param axis_x X component of the rotation axis
         * @param axis_y Y component of the rotation axis
         * @param axis_z Z component of the rotation axis
         * @param angle rotation angle around the given axis in radians
         */
        inline void set_direction(float axis_x, float axis_y, float axis_z, float angle = 0.0f) const
        {
            volumeshOS::set_direction(*this, axis_x, axis_y, axis_z, angle);
        }

        /**
         * Set the orientation of this shape using an axis and an angle. The axis is up (0, 1, 0) by default
         *
         * @tparam Vec3T 3D Vector type
         * @param axis rotation axis
         * @param angle rotation angle around the given axis in radians
         */
        template<typename Vec3T>
        inline void set_direction(const Vec3T& axis, float angle = 0.0f) const
        {
            volumeshOS::set_direction<Vec3T>(*this, axis, angle);
        }

        /**
         * Set the scale of this shape by axis. Default is (1, 1, 1)
         *
         * @param x X scale
         * @param y Y scale
         * @param z Z scale
         */
        inline void set_scale(float x, float y, float z) const
        {
            volumeshOS::set_scale(*this, x, y, z);
        }

        /**
         * Set the scale of this shape for all axis. Default is (1, 1, 1)
         *
         * @param scalar scale value
         */
        inline void set_scale(float scalar) const
        {
            volumeshOS::set_scale(*this, scalar);
        }

        /**
         * Set the scale of this shape by axis. Default is (1, 1, 1)
         *
         * @tparam Vec3T 3D Vector type
         * @param scale scale vector
         */
        template<typename Vec3T>
        inline void set_scale(const Vec3T& scale) const
        {
            volumeshOS::set_scale<Vec3T>(*this, scale);
        }

        /**
         * Set the color of this shape. The default color is white (1, 1, 1, 1)
         *
         * @tparam Vec4T 4D Vector type
         * @param color vector containing the rgba components
         */
        template<typename Vec4T>
        inline void set_color(const Vec4T& color) const
        {
            volumeshOS::set_color<Vec4T>(*this, color);
        }

        /**
         * Returns the current position of this shape
         *
         * @tparam Vec3T 3D Vector type
         * @return current position
         */
        template<typename Vec3T>
        [[nodiscard]] inline Vec3T get_position() const
        {
            return volumeshOS::get_position<Vec3T>(*this);
        }

        /**
         * Returns the current scale of this shape
         *
         * @tparam Vec3T 3D Vector type
         * @return current scale
         */
        template<typename Vec3T>
        [[nodiscard]] inline Vec3T get_scale() const
        {
            return volumeshOS::get_scale<Vec3T>(*this);
        }

        /**
         * Returns the internal volumeshOS id of this shape
         *
         * @return internal id
         */
        [[nodiscard]] int get_id() const
        {
            return m_id;
        }

    private:
        int m_id;
    };

    /**
     * Represents a sphere shape. Use as template parameter to add it to the scene
     */
    struct VSphere : public VShape
    {
        using VShape::VShape;
    };

    /**
     * Represents a cylinder shape. Use as template parameter to add it to the scene.
     * The two ends lie on the world y-axis by default (upwards).
     */
    struct VCylinder : public VShape
    {
        using VShape::VShape;
    };

    /**
     * Represents a box shape. Use as template parameter to add it to the scene
     */
    struct VBox : public VShape
    {
        using VShape::VShape;
    };

    /**
     * Represents a cone shape. Use as template parameter to add it to the scene.
     * The tip and bottom lie on the world y-axis by default (upwards).
     */
    struct VCone : public VShape
    {
        using VShape::VShape;
    };

    /**
     * Represents an arrow shape. Use as template parameter to add it to the scene.
     * The tip points upwards (0, 1, 0) in world coordinates by default.
     */
    struct VArrow : public VShape
    {
        using VShape::VShape;

        /**
         * Set the start of the arrow tip in percentage (between 0 and 1). So a height of 0.7 means 30% tip and 70% base
         *
         * @param tip_height height of the arrow tip in percentage
         */
        inline void set_tip_height(float tip_height) const
        {
            volumeshOS::set_tip_height(*this, tip_height);
        }

        /**
         * Set the width of the arrow base in percentage (default 0.5). So a width of 0.3 means 30% of the tip width
         *
         * @param base_width width of the arrow base in percentage
         */
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

        /**
         * Set mesh from ovm instance without generating a new id.
         * @param instance
         */
        inline void update(OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>* instance) const
        {
            volumeshOS::update(*this, instance);
        }

        /**
         * Set mesh from file without generating a new id.
         * @param path file path
         */
        inline void update(const std::string& path) const
        {
            volumeshOS::update(*this, path);
        }

        /**
         * Remove mesh from volumeshOS.
         */
        inline void clear() const
        {
            volumeshOS::clear(*this);
        }

        /**
         * Returns the OpenVolumeMesh instance of a mesh.
         * @return ovm instance
         */
        [[nodiscard]] inline OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>* get_ovm() const
        {
            return volumeshOS::get_ovm(*this);
        }

        /**
         * Load a configuration file for a mesh. This way settings can be saved across several program executions.
         * @param path path to config file
         */
        inline void load_configuration(const std::string& path) const
        {
            volumeshOS::load_configuration(*this, path);
        }

        /**
         * Save a configuration file for a mesh. This way settings can be saved across several program executions.
         * @param path path to config file
         */
        inline void save_configuration(const std::string& path) const
        {
            volumeshOS::save_configuration(*this, path);
        }

        /**
         * Set Rendering Mode of this mesh.
         * @param mode
         * Points: Rendering vertices aka points \n
         * Lines: Rendering edges aka wireframe \n
         * Cells : Rendering cells (standard) \n
         */
        inline void set_rendering_mode(const RenderingMode mode) const
        {
            volumeshOS::set_rendering_mode(*this, mode);
        }

        /**
        * Get Rendering Mode of this mesh.
         * @return
         * Points: Rendering vertices aka points \n
         * Lines: Rendering edges aka wireframe \n
         * Cells : Rendering cells (standard) \n
         */
        [[nodiscard]] inline RenderingMode get_rendering_mode() const
        {
            return volumeshOS::get_rendering_mode(*this);
        }

        /**
         * Set Point Size of this mesh. Determines the size of rendered points when using ::RenderingMode = Points.
         * @param size point size
         */
        inline void set_point_size(const float size) const
        {
            volumeshOS::set_point_size(*this, size);
        }

        /**
         * Get Point Size of this mesh.
         * @return
         */
        [[nodiscard]] inline float get_point_size() const
        {
            return volumeshOS::get_point_size(*this);
        }

        /**
         * Set Line Width of this mesh. Determines the width of the rendered lines when using ::RenderingMode = Lines.
         * @param width line width
         */
        inline void set_line_width(const float width) const
        {
            volumeshOS::set_line_width(*this, width);
        }

        /**
         * Get Line Width of this mesh.
         * @return
         */
        [[nodiscard]] inline float get_line_width() const
        {
            return volumeshOS::get_line_width(*this);
        }

        /**
         * Set Shading Mode of this mesh.
         * @param mode
         * Flat: Rendering mesh using flat shading \n
         * Phong: Rendering mesh using phong shading \n
         */
        inline void set_shading_mode(const ShadingMode mode) const
        {
            volumeshOS::set_shading_mode(*this, mode);
        }

        /**
         * Get Shading Mode of this mesh.
         * @return
         * Flat: Rendering mesh using flat shading \n
         * Phong: Rendering mesh using phong shading \n
         */
        [[nodiscard]] inline ShadingMode get_shading_mode() const
        {
            return volumeshOS::get_shading_mode(*this);
        }

        /**
         * Enable/Disable backface culling for a mesh. In special cases some faces wants to be seen from behind.
         * @param culling
         */
        inline void use_backface_culling(const bool culling) const
        {
            volumeshOS::use_backface_culling(*this, culling);
        }

        /**
         * True if backface culling is enabled for mesh
         * @return is_using_backface_culling
         */
        [[nodiscard]] inline bool is_using_backface_culling() const
        {
            return volumeshOS::is_using_backface_culling(*this);
        }

        /**
         * True if two sided lighting is enabled for mesh
         * @return is_using_two_sided_lighting
         */
        [[nodiscard]] inline bool is_using_two_sided_lighting() const
        {
            return volumeshOS::is_using_two_sided_lighting(*this);
        }

        /**
         * Enable/Disable two sided lighting for a mesh.
         * @param mesh
         * @param ts_lighting
         */
        inline void use_two_sided_lighting(const bool ts_lighting) const
        {
            volumeshOS::use_two_sided_lighting(*this, ts_lighting);
        }

        /**
         * True if two sided lighting is enabled for mesh
         * @return is_using_two_sided_lighting
         */
        [[nodiscard]] inline bool is_using_base_color() const
        {
            return volumeshOS::is_using_base_color(*this);
        }

        /**
         * Enable/Disable base color for a mesh.
         * When enabled the color for the whole mesh can be specified in the GUI. Otherwise ever cell has its own color.
         * @param base_color
         */
        inline void use_base_color(bool base_color)const
        {
            volumeshOS::use_base_color(*this, base_color);
        }


        /**
         * Set color for a meshes
         * @tparam Vec4T 4D vector type
         * @param color color vector in range [0,1]
         */
        template<typename Vec4T>
        inline void set_color(const Vec4T& color) const
        {
            volumeshOS::set_color<Vec4T>(*this, color);
        }

        /**
         * Set color for one meshes
         * @tparam Vec4T 4D vector type
         * @param cell OVM Cell handle
         * @param color vector in range [0,1]
         */
        template<typename Vec4T>
        inline void set_color(OpenVolumeMesh::CellHandle cell, const Vec4T& color) const
        {
            volumeshOS::set_color<Vec4T>(*this, cell, color);
        }

        /**
         * Set color for one meshes
         * @tparam Vec4T 4D vector type
         * @param face OVM Face handle
         * @param color color vector in range [0,1]
         */
        template<typename Vec4T>
        inline void set_color(OpenVolumeMesh::FaceHandle face, const Vec4T& color) const
        {
            volumeshOS::set_color<Vec4T>(*this, face, color);
        }

        /**
         * Set color for one meshes
         * @tparam Vec4T 4D vector type
         * @param mesh
         * @param halfface OVM Halfface handle
         * @param color vector in range [0,1]
         */
        template<typename Vec4T>
        inline void set_color(OpenVolumeMesh::HalfFaceHandle halfface, const Vec4T& color) const
        {
            volumeshOS::set_color<Vec4T>(*this, halfface, color);
        }


        /**
         * Get color of mesh
         * @tparam Vec4T 4D vector type
         * @return color vector in range [0,1]
         */
        template<typename Vec4T>
        [[nodiscard]] inline Vec4T get_color() const
        {
            return volumeshOS::get_color<Vec4T>(*this);
        }

        /**
         * Get color of a cell
         * @tparam Vec4T 4D vector type
         * @param cell OVM Cell handle
         * @return color vector in range [0,1]
         */
        template<typename Vec4T>
        [[nodiscard]] inline Vec4T get_color(OpenVolumeMesh::CellHandle cell) const
        {
            return volumeshOS::get_color<Vec4T>(*this, cell);
        }

        /**
         * Get color of a halfface
         * @tparam Vec4T 4D vector type
         * @param halfface
         * @return color vector in range [0,1]
         */
        template<typename Vec4T>
        [[nodiscard]] inline Vec4T get_color(OpenVolumeMesh::HalfFaceHandle halfface) const
        {
            return volumeshOS::get_color<Vec4T>(*this, halfface);
        }

        /**
         * Select a Cell. Selected Cells gets highlighted and callbacks are called.
         * @param cell OVM Cell handle
         */
        inline void select(OpenVolumeMesh::CellHandle cell) const
        {
            volumeshOS::select(*this, cell);
        }

        /**
         * Select a Face. Selected Faces gets highlighted and callbacks are called.
         * @param face OVM Face handle
         */
        inline void select(OpenVolumeMesh::FaceHandle face) const
        {
            volumeshOS::select(*this, face);
        }

        /**
         * Select a Halfface. Selected Halffaces gets highlighted and callbacks are called.
         * @param halfface OVM Halfface handle
         */
        inline void select(OpenVolumeMesh::HalfFaceHandle halfface) const
        {
            volumeshOS::select(*this, halfface);
        }

        /**
         * Select a Edge. Selected Edges gets highlighted and callbacks are called.
         * @param edge OVM Edge handle
         */
        inline void select(OpenVolumeMesh::EdgeHandle edge) const
        {
            volumeshOS::select(*this, edge);
        }

        /**
         * Select a Vertex. Selected Vertices gets highlighted and callbacks are called.
         * @param vertex OVM Vertex handle
         */
        inline void select(OpenVolumeMesh::VertexHandle vertex) const
        {
            volumeshOS::select(*this, vertex);
        }


        /**
         * Deselect a Cell.
         * @param cell OVM Cell handle
         */
        inline void deselect(OpenVolumeMesh::CellHandle cell) const
        {
            volumeshOS::deselect(*this, cell);
        }

        /**
         * Deselect a Face.
         * @param face OVM Face handle
         */
        inline void deselect(OpenVolumeMesh::FaceHandle face) const
        {
            volumeshOS::deselect(*this, face);
        }

        /**
         * Deselect a Halfface.
         * @param halfface OVM Halfface handle
         */
        inline void deselect(OpenVolumeMesh::HalfFaceHandle halfface) const
        {
            volumeshOS::deselect(*this, halfface);
        }

        /**
         * Deselect a Edge.
         * @param edge OVM Edge handle
         */
        inline void deselect(OpenVolumeMesh::EdgeHandle edge) const
        {
            volumeshOS::deselect(*this, edge);
        }

        /**
         * Deselect a Vertex.
         * @param edge OVM Vertex handle
         */
        inline void deselect(OpenVolumeMesh::VertexHandle vertex) const
        {
            volumeshOS::deselect(*this, vertex);
        }

        /**
         * Deselect every selected entity.
         * @param mesh
         */
        inline void reset_selection() const
        {
            volumeshOS::reset_selection(*this);
        }


        /**
         * Set ambient term for Phong Lighting.
         * @param ambient value in range [0,1]
         */
        inline void set_ambient(float ambient) const
        {
            volumeshOS::set_ambient(*this, ambient);
        }

        /**
         * Get ambient term for Phong Lighting.
         * @return ambient value in range [0,1]
         */
        [[nodiscard]] inline float get_ambient() const
        {
            return volumeshOS::get_ambient(*this);
        }

        /**
         * Set diffuse term for Phong Lighting.
         * @param diffuse value in range [0,1]
         */
        inline void set_diffuse(float diffuse) const
        {
            volumeshOS::set_diffuse(*this, diffuse);
        }

        /**
         * Get diffuse term for Phong Lighting.
         * @return diffuse value in range [0,1]
         */
        [[nodiscard]] inline float get_diffuse() const
        {
            return volumeshOS::get_diffuse(*this);
        }

        /**
         * Set specular term for Phong Lighting.
         * @param specular value in range [0,1]
         */
        inline void set_specular(float specular) const
        {
            volumeshOS::set_specular(*this, specular);
        }

        /**
         * Get specular term for Phong Lighting.
         * @return specular value in range [0,1]
         */
        [[nodiscard]] inline float get_specular() const
        {
            return volumeshOS::get_specular(*this);
        }

        /**
         * Set specular coefficient for Phong Lighting.
         * @param coefficient value in range [0,10]
         */
        inline void set_specular_coefficient(float coefficient) const
        {
            volumeshOS::set_specular_coefficient(*this, coefficient);
        }

        /**
         * Get specular coefficient for Phong Lighting.
         * @return coefficient value in range [0,10]
         */
        [[nodiscard]] inline float get_specular_coefficient() const
        {
            return volumeshOS::get_specular_coefficient(*this);
        }

        /**
         * Set Lighting mode for a mesh.
         * @param mode
         * PHONG: Phong lighting. Affected by ambient, diffuse, specular term.
         * PBR: Physically Based Rendering. Affected by metallic and roughness term.
         */
        inline void set_lighting_mode(LightingMode mode) const
        {
            volumeshOS::set_lighting_mode(*this, mode);
        }

        /**
         * Get Lighting mode of a mesh
         * PHONG: Phong lighting. Affected by ambient, diffuse, specular term.
         * PBR: Physically Based Rendering. Affected by metallic and roughness term.
         * @return
         */
        [[nodiscard]] inline LightingMode get_lighting_mode() const
        {
            return volumeshOS::get_lighting_mode(*this);
        }

        /**
         * Set metallic term for PBR Lighting.
         * @param metallic value in range [0,1]
         */
        inline void set_metallic(float metallic) const
        {
            volumeshOS::set_metallic(*this, metallic);
        }

        /**
         * Get metallic term for PBR Lighting.
         * @return metallic value in range [0,1]
         */
        [[nodiscard]] inline float get_metallic() const
        {
            return volumeshOS::get_metallic(*this);
        }

        /**
         * Set roughness term for PBR Lighting.
         * @param roughness value in range [0,1]
         */
        inline void set_roughness(float roughness) const
        {
            volumeshOS::set_roughness(*this, roughness);
        }

        /**
         * Get roughness term for PBR Lighting.
         * @param mesh
         * @return roughness value in range [0,1]
         */
        [[nodiscard]] inline float get_roughness() const
        {
            return volumeshOS::get_roughness(*this);
        }

        /**
         * Set the position of a mesh in world coordinates.
         * @param x X coordinate
         * @param y Y coordinate
         * @param z Z coordinate
         */
        inline void set_position(float x, float y, float z) const
        {
            volumeshOS::set_position(*this, x, y, z);
        }


        /**
         * Set position of a mesh in world coordinates.
         * @tparam Vec3T 3D vector type
         * @param position coordinate vector
         */
        template<typename Vec3T>
        inline void set_position(const Vec3T& position) const
        {
            volumeshOS::set_position<Vec3T>(*this, position);
        }

        /**
         * Get position of a mesh in world coordinates.
         * @tparam Vec3T 3D vector type
         * @return position coordinate vector
         */
        template<typename Vec3T>
        [[nodiscard]] inline Vec3T get_position() const
        {
            return volumeshOS::get_position<Vec3T>(*this);
        }

        /**
         * Set scale of a mesh.
         * @param scale
         */
        inline void set_scale(float scale) const
        {
            volumeshOS::set_scale(*this, scale);
        }

        /**
         * Get scale of mesh
         * @param mesh
         * @return scale
         */
        [[nodiscard]] inline float get_scale() const
        {
            return volumeshOS::get_scale(*this);
        }

        /**
         * Set the rotation of a mesh using euler angles
         * @param x
         * @param y
         * @param z
         */
        inline void set_rotation(float x, float y, float z) const
        {
            volumeshOS::set_rotation(*this, x, y, z);
        }

        /**
         * Set the rotation of a mesh.
         * @tparam Vec3T 3D vector type
         * @param rotation vector in range [0,1]
         */
        template<typename Vec3T>
        inline void set_rotation(const Vec3T& rotation) const
        {
            volumeshOS::set_rotation<Vec3T>(*this, rotation);
        }

        /**
         * Get the rotation of a mesh.
         * @tparam Vec3T 3D vector type
         * @return rotation vector in range [0,1]
         */
        template<typename Vec3T>
        [[nodiscard]] inline Vec3T get_rotation() const
        {
            return volumeshOS::get_rotation<Vec3T>(*this);
        }

        /**
         * Get point transform
         * @tparam Vec3T 3D vector type
         * @param point
         * @return
         */
        template<typename Vec3T>
        [[nodiscard]] inline Vec3T get_transformed_point(const Vec3T& point) const
        {
            return volumeshOS::get_transformed_point<Vec3T>(*this, point);
        }

        /**
         * Resets the rotation of a mesh.
         */
        inline void reset_rotation() const
        {
            volumeshOS::reset_rotation(*this);
        }

        /**
         * Set scale of a mesh.
         * @param factor value in range [0,1]
         */
        inline void set_slice_factor(float factor) const
        {
            volumeshOS::set_slice_factor(*this, factor);
        }

        /**
         * Get slice factor
         * @return value in range [0,1]
         */
        [[nodiscard]] inline float get_slice_factor() const
        {
            return volumeshOS::get_slice_factor(*this);
        }

        /**
         * Lock the direction of the slice plane. Aligned by camera view direction.
         * @param lock
         */
        inline void set_slice_locked(bool lock) const
        {
            volumeshOS::set_slice_locked(*this, lock);
        }

        /**
         * True when slicing is locked.
         * @return slice_locked
         */
        [[nodiscard]] inline bool get_slice_lock() const
        {
            return volumeshOS::get_slice_locked(*this);
        }

        /**
         * Set the peel level for a given mesh.
         * @param level value between 0 (no peel) and the total number of depth layers in the mesh.
         *              Float values can be displayed transparent when transparency is active.
         *              Values above the maximum peel level are set to maximumum peel level
         */
        inline void set_peel_level(float level) const
        {
            volumeshOS::set_peel_level(*this, level);
        }

        /**
         * Get peel level of a mesh
         * @return value between 0 (no peel) and the total number of depth layers in the mesh.
         */
        [[nodiscard]] inline float get_peel_level() const
        {
            return volumeshOS::get_peel_level(*this);
        }

        /**
         * Enable/Disable reverse peeling for a mesh. Inner layers gets peeled first.
         * @param reverse
         */
        inline void use_reverse_peeling(bool reverse) const
        {
            volumeshOS::use_reverse_peeling(*this, reverse);
        }

        /**
         * Returns maximum peel depth. Depends on mesh.
         * @return maximum peel depth
         */
        [[nodiscard]] inline bool is_using_reverse_peeling() const
        {
            return volumeshOS::is_using_reverse_peeling(*this);
        }

        /**
         * Returns maximum peel depth. Depends on mesh.
         * @return maximum peel depth
         */
        [[nodiscard]] inline int get_max_peel_depth() const
        {
            return volumeshOS::get_max_peel_depth(*this);
        }


        /**
         * Set the rounding factor for each cell of a mesh.
         * @param rounding 0 (no rounding) to 1 (full rounding).
         */
        inline void set_cell_rounding(float rounding) const
        {
            volumeshOS::set_cell_rounding(*this, rounding);
        }

        /**
         * Get the rounding factor of a mesh.
         * @return rounding 0 (no rounding) to 1 (full rounding).
         */
        [[nodiscard]] inline float get_cell_rounding() const
        {
            return volumeshOS::get_cell_rounding(*this);
        }

        /**
         * Get the tessellation level for Bézier meshes (non-Bézier meshes are not affected)
         * @return level value in range [0,64]
         */
        [[nodiscard]] inline int get_tessellation_level() const
        {
            return volumeshOS::get_tessellation_level(*this);
        }

        /**
         * Set the tessellation level for Bézier meshes (non-Bézier meshes are not affected)
         * @param level value in range [1,64]
         */
        inline void set_tessellation_level(int level) const
        {
            volumeshOS::set_tessellation_level(*this, level);
        }

        /**
         * Set the cell size of a given mesh.
         * @param rounding 0 (infinitely small) to 1 (original size).
         */
        inline void set_cell_size(float size) const
        {
            volumeshOS::set_cell_size(*this, size);
        }

        /**
         * Get the cell size of a given mesh.
         * @return rounding 0 (infinitely small) to 1 (original size)
         */
        [[nodiscard]] inline float get_cell_size() const
        {
            return volumeshOS::get_cell_size(*this);
        }

        /**
         * Get given mesh into focus. Some operations only occur on the focused mesh.
         * @return
         */
        inline void set_focused() const
        {
            volumeshOS::set_focused_mesh(*this);
        }

        /**
         * Check if this mesh is currently in focus.
         *
         * @return true if focused, else false
         */
        [[nodiscard]] inline bool is_focused() const
        {
            return volumeshOS::get_focused_mesh().m_id == m_id;
        }

        /**
         * Set Cell (in-)visibility.
         * @param cell OVM Cell handle
         * @param visible
         */
        inline void set_visibility(OpenVolumeMesh::CellHandle cell, bool visible) const
        {
            volumeshOS::set_visibility(*this, cell, visible);
        }

        /**
         * Get cell (in-)visibility
         * @param cell
         * @return visible
         */
        [[nodiscard]] inline bool get_visibility(OpenVolumeMesh::CellHandle cell) const
        {
            return volumeshOS::get_visibility(*this, cell);
        }

        /**
         * Set mesh (in-)visible.
         * @param visible
         */
        inline void set_visibility(bool visible) const
        {
            volumeshOS::set_visibility(*this, visible);
        }

        /**
         * Get mesh (in-)visible.
         * @return visible
         */
        [[nodiscard]] inline bool get_visibility() const
        {
            return volumeshOS::get_visibility(*this);
        }

        /**
         * Set all cells visible.
         */
        inline void reset_visibility() const
        {
            volumeshOS::reset_visibility(*this);
        }

        /**
         * Isolate a single cell of a mesh, making it the only visible cell.
         * @param cell OVM Cell handle
         */
        inline void isolate(OpenVolumeMesh::CellHandle cell) const
        {
            volumeshOS::isolate(*this, cell);
        }

        /**
         * Dig a single cell of a mesh, making it invisible.
         * @param cell OVM Cell handle
         */
        inline void dig(OpenVolumeMesh::CellHandle cell) const
        {
            volumeshOS::dig(*this, cell);
        }

        /**
         * Set name for mesh
         * @param name name string
         */
        inline void set_name(const std::string& name) const
        {
            volumeshOS::set_name(*this, name);
        }

        /**
         * Get name for mesh
         * @param name name string
         */
        [[nodiscard]] inline const std::string& get_name() const
        {
            return volumeshOS::get_name(*this);
        }

        /**
         * Add shape to mesh
         * @tparam ShapeType VArrow,VBox,VSphere,VCylinder,VCone
         * @return VShape object
         */
        template<typename ShapeType>
        [[nodiscard]] inline ShapeType add_shape() const
        {
            static_assert(std::is_base_of_v<VShape, ShapeType>);
            return volumeshOS::add_shape<ShapeType>(*this);
        }

        /**
         * Add shape to cell. This way they share properties with it (like visibility and size).
         * @tparam ShapeType VArrow,VBox,VSphere,VCylinder,VCone
         * @param cell OVM cell handle
         * @return VShape object
         */
        template<typename ShapeType>
        [[nodiscard]] inline ShapeType add_shape(OpenVolumeMesh::CellHandle cell) const
        {
            static_assert(std::is_base_of_v<VShape, ShapeType>);
            return volumeshOS::add_shape<ShapeType>(*this, cell);
        }


        /**
         * Returns true if the mesh is a valid handle.
         * @return validity of VMesh
         */
        [[nodiscard]] inline bool is_valid() const
        {
            return volumeshOS::is_valid(*this);
        }

        /**
         * True if the mesh is a Bézier mesh
         * @return is_bezier
         */
        [[nodiscard]] inline bool is_bezier_mesh() const
        {
            return volumeshOS::is_bezier_mesh(*this);
        }

        /**
         * Returns the internal volumeshOS id of this mesh
         * @return internal id
         */
        [[nodiscard]] int get_id() const
        {
            return m_id;
        }

    private:
        int m_id;
    };

} // namespace volumeshOS
