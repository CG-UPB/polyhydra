# Sky
In order to beautify the ambience, the background can be changed by setting a sky color.
It also affects physically based rendering (PBR). 
In addition to that there is also a fog with a certain color and density.

```cpp
// sky color
std::array<float, 4> sky_col = {0.5f, 0.75f, 0.8f};
set_sky_color(sky_col);

// fog density
set_fog_density(0.05f);

// fog color
std::array<float, 4> fog_col = {0.5f, 0.75f, 0.8f};
set_fog_color(fog_col);
```

