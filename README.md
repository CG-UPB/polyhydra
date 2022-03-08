# volumeshOS

## Content
- [Getting Started](#getting-started)
- [How to Use](#how-to-use)
- [In-Built Features](#inbuild-features)


## Getting Started

## How to Use

## Inbuild Features
- [Mesh List](#mesh-list)
- [Toolbar](#toolbar)
- [Snapshot](#snapshot)
- [Mesh Settings](#mesh-settings)
- [Log-Window](#log-window)

### Mesh-List
> TODO: Bild einfügen

The mesh list is a built-in feature that lists all existing meshes. Besides listing all the meshes, the mesh list also has the functionality to give the meshes different settings. These can be changed via the buttons and checkboxes next to the name of the meshes, which are each named after their program-internal ID. In addition, the mesh list offers the possibility to set a mesh in focus by double-clicking the corresponding radio button. A single click makes the respective mesh the active mesh, which can then be modified by the [toolbar](#toolbar) operations.

### Toolbar
<center>
<img src="res/documentation_images/Toolbar.png" alt="drawing" width="200"/>
</center>

Another integrated feature is the toolbar. Various actions are possible in this toolbar. On the one hand there is the possibility to use the [snapshot](#snapshot) function to save snapshots via a file dialog. Another possibility is the activation of the selection feature, through which the individual elements of the meshes can be selected. It is also possible to specify whether only vertices, edges, faces or cells can be selected. Below this, on the toolbar, is the tab for the [mesh settings](#mesh-settings), which are always displayed and applied to the active mesh.


### Snapshot
The snapshot feature is a button on the [toolbar](#toolbar), which creates a snapshot of the actual viewport. This opens a file dialog where you can specify a file to save the snapshot to. The file dialog is the native dialog of the operating system used. The supported file formats are '.png' and '.bmp'. The screenshot is created with a transparent background, so that a created image can be used further. If the user wants to make snapshots via code he can call the method *take_screenshot(string path)* from the class Window. A call would look like this:
> Window::instance()->take\_screenshot(<your_filename.png>)

Thus, the user can create automated snapshots

### Mesh Settings
The Mesh Settings are settings that affect the individual meshes. They are intended to provide a better view of the meshes and are too complex to be included in the [Mesh List](#mesh-list). On the one hand the active mesh can be translated and scaled. On the other hand the cell size can be changed or cells can be shown/hidden after certain operations. For this there is the slice function that slices through the mesh using a layer. The layer is defined depending on the camera, but can be fixed using the adjacent lock button. The peel slider can be used to slice through the mesh layer by layer. The isolation feature isolates individual cells, while the digging feature can delete individual cells. The respective values of the sliders can also be set via code. An example call for the peel level looks like this:

> Window::instance->set\_peel\_level(int mesh\_id, int level);

Thus, the user can also use values calculated in the code to display the meshes differently. As you cn see, the user has also the possibility to set the peel_level of a non-active mesh, which could be useful at some time


### LogWindow
The LogWindow uses the singleton pattern, thus all classes interact with the same logger. A statement can be printed onto the logger console by use of the function

> void addLog(const char* fmt, int level = 0);

where fmt is the statement to be printed, and level denotes the criticality. The criticality is seperated in the levels: information, warning, error, critical. The default value is information. It might be useful to write the calling classes' name into the print-statement.
In the created Logwindow we provide some buttons for the User of the program. There is a clear button, which clears the
Logwindow, so that every line is deleted and the buffer is empty. The next button copy, copies the buffer into the clipboard
of the User. The button options provides some options for the Logwindow (at the moment only the autoscroll-feature). At least the User
can use the filter-field. This filters every line of the Logwindow, with the given string, and only shows the line in which the string is include.

To use the LogWindow the programmer has to create log-messages as follows:
> include "../panels/LogWindow.h"
>
> ...
>
> LogWindow::getInstance()->addLog(const char* fmt, int level = 0);

Every log-message will be given in a buffer, and printed in the next iteration of the show-method


## Modes
We provide a selection of available modes that are preset and rendered with the optimized settings. You can change the actual mode in the [toolbar](#toolbar):
<center>
<img src="res/documentation_images/Mode_Selection.png" alt="drawing" width="200"/>
</center>
The following is a listing and specific explanation of each mode:

- [Wireframe](#wireframe-mode)
- [Only Vertices](#only-vertices-mode)
- [Phong Facenormals](#phong-facenormals-mode)
- [Phong Vertexnormals](#phong-vertexnormals-mode)
- [Transparency](#transparency-mode)
- [Rounded](#rounded-mode)
- [Ambient Occlusion](#ambient-occlusion-mode)
- [Shadows](#shadows-mode)

### Wireframe Mode

### Only Vertices Mode

### Phong Facenormals Mode

### Phong Vertexnormals Mode

### Transparency Mode

### Rounded Mode

### Ambient Occlusion Mode

### Shadows Mode