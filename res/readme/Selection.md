# Selection

```cpp
set_selection_mode(SelectionMode::CELL);

on_cell_select([](VMesh mesh, OpenVolumeMesh::CellHandle ch){
    log("Cell " + std::to_string(ch.uidx()) + " was selected");
});
```
The Selection feature is disabled by default. `OFF`, `VERTEX`, `EDGE`, `FACE`, `CELL` and `ALL` are selectable.
In addition to that the programmer can define a callback function that is called when the corresponding entity is selected. 
In this case we use the internal logging system to output the index of the selected cell.
Other possibilities are to change the color or append a shape at the entity's position.

<div style="display:flex; justify-content:space-between;">
    <img src="../readme_res/selection_off.png" alt="Bild 1" style="width:19.5%;">
    <img src="../readme_res/selection_vertex.png" alt="Bild 2" style="width:19.5%;">
    <img src="../readme_res/selection_edge.png" alt="Bild 3" style="width:19.5%;">
    <img src="../readme_res/selection_face.png" alt="Bild 4" style="width:19.5%;">
    <img src="../readme_res/selection_cell.png" alt="Bild 5" style="width:19.5%;">
</div>

***
[Table of Content](TableOfContent.md)