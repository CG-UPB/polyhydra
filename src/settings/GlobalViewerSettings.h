#pragma once

#include "../panels/ToolBar.h"

namespace vOS
{
    class GlobalViewerSettings
    {
    public:
        ~GlobalViewerSettings();
        static GlobalViewerSettings* getInstance();

        void m_set_current_selection_mode(int mode){m_current_selection_mode = mode;};
        void m_set_current_mesh_rendering_color(float r, float g, float b, float a){m_color[0] = r; m_color[1] = g; m_color[2] = b, m_color[3] = a;};
        void m_set_current_mesh_slice_level(float slice_lvl){m_slider_slicer = slice_lvl;};
        void m_set_current_mesh_peel_level(float peel_lvl){m_slider_peel = peel_lvl;};
        void m_set_current_isolation_state(bool isolation_state){m_isolation_started = isolation_state;};
        void m_set_current_separation_type(int separation_type){m_separation_type = separation_type;};
        void m_set_current_lighting_type(int lighting_type){m_lighting_type = lighting_type;};
        void m_set_current_rendering_mode(int rend_mode){m_rendering_mode = rend_mode;};

        int m_get_current_selection_mode(){return m_current_selection_mode;};
        float* m_get_current_mesh_rendering_color(){return m_color;};
        float m_get_current_mesh_slice_level(){return m_slider_slicer;};
        float m_get_current_mesh_peel_level(){return m_slider_peel;};
        bool m_get_current_isolation_state(){return m_isolation_started;};
        int m_get_current_separation_type(){return m_separation_type;};
        int m_get_current_lighting_type(){return m_lighting_type;};
        int m_get_current_rendering_mode(){return m_rendering_mode;};

    private:
        GlobalViewerSettings();
        static GlobalViewerSettings* instance;

        int m_current_selection_mode;
        float m_color[4] = {1.0f,0.0f,0.0f,1.0f};
        float m_slider_slicer = 0.5f;
        float m_slider_peel = 0.5f;
        bool m_isolation_started = false;
        int m_separation_type = 2;
        int m_lighting_type = 3;
        int m_rendering_mode = 1;
    };

}
