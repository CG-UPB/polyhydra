# Ground

```cpp
use_ground(true);
set_ground_color(std::array<float,3>{1.0, 1.0, 1.0});

use_grid(true);
set_grid_color(std::array<float,3>{0.5, 0.5, 0.5});

set_ground_height(-5.0f);
```

The floor consists of a solid part and a grid. Both can be assigned an individual color. The height counts for both. The solid ground receives shadows.
