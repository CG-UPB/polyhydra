[Table of Content](TableOfContent.md)
***

# Your First Program

The simplest Polyhydra program looks like this:
```c
#include "polyhydra.h"

using namespace polyhydra;

int main()
{
    load("path/to/ovm_file.ovm");
    open();
}
```
First include `polyhydra.h`. Then load a mesh and open the viewer.

<div style="display:flex; justify-content:space-between;">
    <img src="../readme_res/viewer.png" alt="Bild 1" style="width:100%;">
</div>
<figcaption style="width: 100%;">
    Example view
</figcaption>
