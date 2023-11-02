# Light
VolumeshOS has one global light. Its direction and color affects lighting and shadows and also the sky gradient.

```cpp
// direction
std::array<float, 4> dir = {0.5f, 1.0f, 1.0f};
set_light_direction(dir);

// color
std::array<float, 4> col = {1.0f, 1.0f, 1.0f};
set_light_color(col);

//intensity
set_light_intensity(5.0f);
```
