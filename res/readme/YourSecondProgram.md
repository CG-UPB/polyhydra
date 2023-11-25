# Your Second Program

## Custom Wireframe with Cylinders
The following Code adds a custom Wireframe out of many cylinders:
```c
for(auto c_it : ovm_mesh->cells())
{
    for(auto e_it : ovm_mesh->cell_edges(c_it))
    {
        // get both vertices of the edge
        auto edge = ovm_mesh->edge(e_it);
        auto from = ovm_mesh->vertex(edge.from_vertex());
        auto to = ovm_mesh->vertex(edge.to_vertex());
        
        // calculate direction
        auto dir = to - from;
        auto pos = from + (dir / 2.0f);
        
        // add cylinder 
        auto cylinder = mesh.add_shape<VCylinder>(c_it);
        cylinder.set_position(pos);
        float thickness = 0.1f;
        cylinder.set_scale(thickness, glm::length(glm::vec3(dir[0], dir[1], dir[2])), thickness);
        cylinder.set_direction(dir);
    }
}
```
For each cell we iterate over all of its edges. After that a direction between both vertices is calculated.
After adding a cylinder, we set its position and scale it to the length of the edge. This can be done by scaling the shape
along its Y-axis. The X-Axis and Z-Axis are responsible for the thickness.

***
[Table of Content](TableOfContent.md)