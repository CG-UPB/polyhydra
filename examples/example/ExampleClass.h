//
// Created by jan on 14.01.22.
//

#ifndef VOLUMESHOS_EXAMPLECLASS_H
#define VOLUMESHOS_EXAMPLECLASS_H

#include "panels/Window.h"

class ExampleClass {
public:
    void simple_demonstration_ui();
    void toolbar_demonstration_ui();
    void selection_demonstration_ui();
    void bounding_demonstration_ui();

    void simple_run();
    void toolbar_run();

    void set_mesh_data();

    void selection_run();

    void bounding_run();

    void start();

    void initialize();
private:
    int m_phase = 0;
    int hand = 0;
    int guy = 0;

    int selection_type = 0;
    int selection_level = 0;

    float bounding_box_thickness = 0.01f;
};


#endif //VOLUMESHOS_EXAMPLECLASS_H
