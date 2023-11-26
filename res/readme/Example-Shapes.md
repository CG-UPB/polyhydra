# Your Second Program

## Wireframe out of Cylinders
The following Code adds a custom Wireframe out of many cylinders:
```c
for(auto c_it : ovm_mesh.cells())
{
    for(auto e_it : ovm_mesh.cell_edges(c_it))
    {
        // get both vertices of the edge
        auto edge = ovm_mesh.edge(e_it);
        auto from = ovm_mesh.vertex(edge.from_vertex());
        auto to = ovm_mesh.vertex(edge.to_vertex());
        
        // calculate direction
        auto dir = to - from;
        auto pos = from + (dir / 2.0f);
        
        // add cylinder
        auto cylinder = mesh.add_shape<VCylinder>(c_it);
        cylinder.set_position(pos);
        float thickness = 0.05f;
        cylinder.set_scale(thickness, dir.length(), thickness);
        cylinder.set_direction(dir);
    }
}
```

![](../readme_res/example_wireframe.png)

For each cell we iterate over all of its edges. After that a direction between both vertices is calculated.
After adding a cylinder, we set its position and scale it to the length of the edge. This can be done by scaling the shape
along its Y-axis. The X-Axis and Z-Axis are responsible for the thickness.


## Normals
The following example is similar to the previous one. Here we visualize face normals with arrow shapes.

```c
for(auto c_it : ovm_mesh.cells())
{
    for(auto f_it : ovm_mesh.cell_faces(c_it))
    {
        for(auto hf_it: ovm_mesh.face_halffaces(f_it))
        {
            if(ovm_mesh.is_boundary(hf_it))
            {
                // get face center
                auto center = ovm_mesh.barycenter(f_it);
                
                // add arrow
                auto arrow = mesh.add_shape<VArrow>(c_it);
                arrow.set_position(center);
                auto dir = ovm_mesh.normal(hf_it);
                arrow.set_direction(dir);
                float thickness = 0.1f;
                arrow.set_scale(Vec3d(thickness, 1.0f, thickness));
                arrow.set_tip_height(0.3f);
            }
        }
    }
}
```

![](../readme_res/example_normals.png)


***
[Table of Content](TableOfContent.md)