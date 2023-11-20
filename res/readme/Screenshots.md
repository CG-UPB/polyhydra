# Screenshots
For Screenshots there is an `ExportOptions` object. 
Currently `.png` are `.bmp` are supported. 
When rendering screenshots, the background, ground, and shapes can be excluded. 
In addition, the shadows of the floor can be separated transparently.


```cpp
ExportOptions options;
options.width = 1280;
options.height = 720;
options.include_background = true;
options.include_shapes = true;
options.include_ground = true;
options.ground_shadow_only = false;

export_image("screenshot.png", options);
```

<div style="display:flex; justify-content:space-between;">
    <img src="../readme_res/mesh_screenshot_complete.png" alt="Bild 1" style="width:50%;">
    <img src="../readme_res/mesh_screenshot_nobg.png" alt="Bild 2" style="width:50%;">
</div>
<div style="display:flex; justify-content:space-between;">
    <img src="../readme_res/mesh_screenshot_nog.png" alt="Bild 1" style="width:50%;">
    <img src="../readme_res/mesh_screenshot_shadow.png" alt="Bild 2" style="width:50%;">
</div>

 