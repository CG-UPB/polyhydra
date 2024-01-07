[Table of Content](TableOfContent.md)
***

# Camera
```cpp
set_camera_mode(CameraMode::ORBIT);
//set_camera_mode(CameraMode::FLY);

set_camera_fov(45.0f);

set_camera_position(std::array<float, 3>{0.0f, 0.0f, -10.0f});

set_camera_target(std::array<float, 3>{0.0f, 0.0f, 0.0f});
```
There are two modes for the camera: `CameraMode::ORBIT` and `CameraMode::FLY`.
In `ORBIT` mode the camera rotates around a given target. In `FLY` mode you can move around freely with Hotkeys `W`,`A`,`S`,`D`,`Shift`,`Space`. The `M` key toggles between both modes and for the `ORBIT` mode the camera take the current mesh as target.
