# Your First Program

The simplest volumeshOS program looks like this:
```c
#include "volumeshOS.h"

using namespace volumeshOS;

int main()
{
    load("path/to/ovm_file");
    open();
}
```
First include `volumeshOS.h`. Then load a mesh and open the viewer. 

![](../readme_res/viewer.png)