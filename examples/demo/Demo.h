//
// Created by jan on 12.03.22.
//

#ifndef VOLUMESHOS_DEMO_H
#define VOLUMESHOS_DEMO_H

#include "panels/Window.h"

using namespace volumeshOS;

class Demo {
public:
    Demo();
    void start();

    void code_demo();
    void code_demo_ui();

    void color_splash_init() ;
    void color_splash_ui();
    void color_splash_hit(int mesh_id, int element_id, bool selected);
    void color_splash_hit_threaded(int mesh_id, int element_id, bool selected);

    void multi_mesh();
    void multi_mesh_ui();
private:
    Window* window;

    int m_nut_mesh;
    int m_hand_mesh;

    float m_mesh_scalar_value = 1.0f;
    float m_splash_size = 5.0f;
    float m_splash_strength = 0.7f;
};


#endif //VOLUMESHOS_DEMO_H
