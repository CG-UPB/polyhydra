# **Information regarding Bézier mesh visualization**

VolumeshOS was adapted to also visualize tetrahedral Bézier meshes.

Example Bézier meshes can be found in `res/sample_meshes/bezier_meshes/`.

For visualizing the control polygons of a Bézier mesh, the example program `"volumeshOS_bezier_select"` can be executed.
In this program Halfface selection must be activated and the option "Show control polygon when selected" must be ticked to visualize the control points of a selected halfface.

## Content
- [Bézier mesh format](#bézier-mesh-format)
- [Changes made to volumeshOS for Bézier mesh visualization](#Changes-made-to-volumeshOS-for-Bézier-mesh-visualization)
- [Completely separating Bézier mesh visualization from the polyhedral mesh visualization](#Completely-separating-Bézier-mesh-visualization-from-the-polyhedral-mesh-visualization)


## **Bézier mesh format**

Bézier meshes are represented as tetrahedral OpenVolumeMesh geometry kernels with certain face property.
The underlying topology of a Bézier mesh is encoded via the underlying OVM geometry kernel.

The control points defining each Bézier tetrahedron's face are stored in a face property named `"BezierFaceControlPoints"` which is attached to the corresponding face of a tetrahedron of the OVM geometry kernel.
This means that control points for a Bézier tetrahedron are stored per face and not per tetrahedron, so that inner control points of a Bézier tetrahedron are not stored with this representation.
The face property is of type `std::vector<double>`, since OpenVolumeMesh is able to serialize this type.
All control points $c_{(i_2, i_1, i_0)}$ of a Bézier tetrahedron's must be stored in the following format in the face property:
$$
\begin{align*}
&c_{(0,0,m)},\ c_{(0,1,m-1)},\hskip 3em ...,\hskip 3em c_{(0,m,0)}, \\
&c_{(1,0,m-1)},\ c_{(1,1,m-2)},\ ...,\ c_{(1,m-1,0)}, \\
& ... \\
&c_{(m-1,0,1)},\ c_{(m-1,1,0)}, \\
&c_{(m, 0,0)}
\end{align*}
$$
where $m$ is the degree of the Bézier triangle and $(i_2, i_1, i_0)$ is the multi-index of each control point, e.g. $c_{(0,0,m)}$, $c_{(0,m,0)}$ and $c_{(m, 0,0)}$ are the "corner control points" of the triangle. 
In this *one-dimensional* vector, each control point is then encoded as its three successive coordinates.

The degree of a Bézier mesh is encoded via a OpenVolumeMesh mesh property of type `int` named `"BezierDegree"`.

Also, an OVM geometry kernel representing a Bézier mesh needs a mesh property of type `bool` named `"BézierMesh"` which is set to `true` in order to be visualized at all.

## **Changes made to volumeshOS for Bézier mesh visualization**

The following is a list of changes applied to volumeshOS in order to visualize Bézier meshes represented as described above.
This information may become useful when debugging, changing, or removing the code added for Bézier mesh visualization.

### `Shader` class
One of the main additions to volumeshOS for Bézier mesh visualization is a tessellation stage consisting of a tessellation control shader (`.tesc`) and a tessellation evaluation shader (`.tese`).
The `Shader` class is adapted in such a way that it automatically loads, compiles and links these tessellation shaders from files with a respective file extension if they can be found (similar to how geometry shaders are loaded).

### `VertexArrayObject` class
If a shader program contains a tessellation stage, `Gl_PATCHES` must be set as the OpenGL primitive for drawing.
For this the `draw_patches` method has been added to the `VertexArrayObject` which is called in a rendering pass instead of `draw` if a tessellation stage is present. 

### Rendering Passes
All shader programs adjusted for Bézier mesh visualization need certain uniform shader variables and a texture buffer which is used for control point access from a shader.

Regarding uniforms, the uniform `u_is_bezier_mesh` always needs to be set, so that the tessellation stage actually tessellates a triangle patch into more sub-patches for Bézier meshes. 
If a Bézier mesh is rendered, also `u_bezier_degree` is set to the Bézier meshes degree and `u_control_points_tb` is set to 12, so that **texture unit 12 is used for the control point texture buffer**.
Also, the tessellation level applied in the tessellation control shader is set as the uniform `u_bezier_tessellation_level`.
Additionally, the `u_rounding` uniform is always set to false for Bézier meshes and the vertex array object used for rounding is never bound for Bézier meshes.

If a Bézier mesh is rendered, the text buffer storing the control points is also bound. 
This texture buffer is wrapped through the new class `MeshTextureBuffer` which automatically creates an OpenGL texture buffer and fills it with the control points of a Bézier mesh's face property, when a OpenVolumeMesh geometry kernel with the mesh property `"BézierMesh"` set to `true` is loaded.

### Shader programs

Bézier meshes are visualized by first tessellating each triangle of the triangle mesh automatically prepared through the `MeshVertexBuffer` and then moving each vertex of the tessellated patch to the correct position in the tessellation evaluation shader by evaluating the de Casteljau algorithm.
For this, to some shader programs a tessellation stage was added (`.tesc`, `.tesc`) which all do fundamentally the same.

The tessellation control shader controls into how many sub-patches each triangle patch is tessellated by setting the tessellation level.
For Bézier meshes, this is the value of the uniform variable `u_bezier_tessellation_level`.
For non-Bézier, polyhedral meshes the inner and outer tessellation levels are always set to 1 which results in no sub-patches being generated.
The tessellation control shader also assigns each vertex output variable to a corresponding tessellation control shader output value without change.

The tessellation evaluation shader then performs the de Casteljau algorithm if a Bézier mesh is rendered and overwrites specific vertex attributes for this.
For this, the built-in barycentric coordinates `gl_TessCoord` are used.
For normal polyhedral meshes, no Bézier triangle evaluation is performed and each tessellation control variable is just interpolated with respect to these barycentric coordinates (which is not really interpolation because it is just done for each of the three corner vertices).

The output variables of the tessellation shader have in general the same names as the corresponding vertex shader output variable names, so that **in general no adaptation of the following shader stages were necessary**.
**However, for the additional visualization features for Bézier meshes, some adjustments were necessary, including the following:**

- For Selection Bézier faces, it was necessary to use the OVM halfface index for indexing the control point texture buffer. Because the `mesh_phong` vertex buffer already declares 16 vertex attributes no further vertex attribute could be added. Because of this, the vertex attribute `a_is_triangle` is used as the variable storing the OVM halfface index if a Bézier mesh is rendered, since this vertex attribute has no use for Bézier mesh rendering. If a Bézier mesh is rendered, this OVM attribute is the passed on as `v_ovm_halfface_id` and `tc_ovm_halfface_id` to the tessellation control shader and tessellation evaluation shader.
- For line rendering, a new shader variable named `v_tesInnerTri` / `v_tes_inner_tri` was added which is passed on in the `mesh_phong` geometry shader to the fragment shader, and it is used in the fragment shader for drawing a Bézier mesh's wireframe.

### `MeshVertexBuffer` class
For Cell sizing, the Point rendering mode for Bézier meshes and for the OVM halfface index, some changes are made to `MeshVertexBuffer::add_cell_by_faces`.
This includes setting the OVM halfface index for each vertex of the corresponding halfface.
For the former two features, as vertices of a Bézier mesh the "corner control points" of each Bézier tetrahedron are used, i.e. the four control points with multi-indices $(0,0,0,m), (0,0,m,0), (0,m,0,0), (m,0,0,0)$, which are retrieved from the meshes face property holding the control points.

### Miscellaneous
- To the `ToolBar` class and static `AppState::settings` structure  additional fields and functions have been added, so that an additional menu entry is rendered below the existing ones to allow the user to change the tessellation level applied in the tessellation control shader for Bézier mesh rendering.
- The new `MeshTextureBuffer` class is instantiated as a member of a `MeshObject` instance.
- The `MeshObject` class has a new method `is_bezier_mesh` to retrieve if a mesh represents a Bézier mesh.

## Completely separating Bézier mesh visualization from the polyhedral mesh visualization
Although no tessellation is performed for normal polyhedral meshes, the tessellation stage is still invoked for them.
Also, the changes applied to volumeshOS' rendering pipeline should be oblivious to the visualization of non-Bézier, polyhedral meshes.
If this still yields a disproportionately much performance decrease, the changes done to volumeshOS could be completely separated from the unchanged volumeshOS rendering pipeline.
This could be done in the following, simplified way:

- Duplicating each shader to which a tessellation stage was added. Then, removing the tessellation stage for one of each pair. For the "reverted" shader also the custom shader changes discussed above would need to be removed.
- In each rendering pass which was adapted for Bézier meshes (uniform set up, texture buffer binding, `draw_patches`) `MeshObject::is_bezier_mesh` can be used to distinguish between a mesh representing a Bézier mesh and a normal polyhedral mesh.
Then, for each Bézier meshes the adjusted rendering function can be called using the shader program with tessellation stage.
For each normal polyhedral mesh, the other shader program with the tessellation stage removed could be used by calling `VertexArrayObject::draw`.
In the latter case, also the preparations for Bézier meshes in the rendering function (uniform set up, texture buffer binding) would need to be removed.