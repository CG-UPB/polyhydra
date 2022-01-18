#pragma once

#include "../panels/ToolBar.h"
#include "../util/shader_enum.h"
#include "../mesh/MeshObject.h"

namespace vOS
{
    class GlobalViewerSettings
    {
    public:
        ~GlobalViewerSettings();
        static GlobalViewerSettings* getInstance();

        void m_new_Mesh();


        void m_set_take_snapshot(bool snap){m_take_snapshot = snap;};
        void m_set_actual_snapshot_filename(std::string name){m_actual_snapshot_filename = name;};
        void m_set_current_selection_activated(bool activated){m_current_selection_activated = activated;};
        void m_set_current_selection_mode(int mode){m_current_selection_mode = mode;};
        void m_set_current_mesh_rendering_color(bool activ, float r, float g, float b, float a){m_color_activated = activ; m_color[0] = r; m_color[1] = g; m_color[2] = b, m_color[3] = a;};
        void m_set_current_isolation_state(bool isolation_state){m_isolation_started = isolation_state;};
        void m_set_current_separation_type(int separation_type){m_separation_type = separation_type;};
        void m_set_current_lighting_type(int lighting_type){m_lighting_type = lighting_type;};
        void m_set_current_new_active_mesh(bool new_active){m_new_active_mesh = new_active;};
        void m_set_current_active_mesh(int active_mesh){m_active_mesh = active_mesh;};
        //void m_set_current_selected_mesh(bool sel[]){m_selected_mesh = sel;};
        void m_set_current_nbr_meshes(int nbr_meshes){m_nbr_meshes = nbr_meshes;};

        void m_set_test(std::vector<bool> sel){m_sel = sel;};


        bool m_get_take_snapshot(){return m_take_snapshot;};
        std::string m_get_actual_snapshot_filename(){return m_actual_snapshot_filename;};
        bool m_get_current_selection_activated(){return m_current_selection_activated;};
        int m_get_current_selection_mode(){return m_current_selection_mode;};
        bool m_get_color_activated(){return m_color_activated;}
        float* m_get_current_mesh_rendering_color(){return m_color;};
        bool m_get_current_isolation_state(){return m_isolation_started;};
        int m_get_current_separation_type(){return m_separation_type;};
        int m_get_current_lighting_type(){return m_lighting_type;};
        int m_get_current_rendering_mode(){return m_rendering_mode;};
        bool m_get_current_new_active_mesh(){return m_new_active_mesh;}
        int m_get_current_active_mesh(){return m_active_mesh;};
        //bool* m_get_current_selected_mesh(){return m_selected_mesh;};
        int m_get_current_nbr_meeshes(){return m_nbr_meshes;};

        std::vector<bool> get_test(){return m_sel;};

        bool get_Visibility_of_Mesh(int i);

        void set_Visibility_of_Mesh(int i, bool b);

    private:
        GlobalViewerSettings();
        static GlobalViewerSettings* instance;

        bool m_take_snapshot = false;
        std::string m_actual_snapshot_filename = "default.png";
        bool m_current_selection_activated = false;
        int m_current_selection_mode = 0;
        bool m_color_activated = true;
        float m_color[4] = {1.0f,1.0f,1.0f,1.0f};
        int m_slider_slicer = 0;
        int m_slider_peel = 0;
        bool m_isolation_started = false;
        int m_separation_type = 2;
        int m_lighting_type = 3;
        int m_rendering_mode = 0;

        bool m_new_active_mesh = false;
        int m_active_mesh = 0;

        bool m_selected_mesh[1] = {true};
        int m_nbr_meshes = 0;

        std::vector<bool> m_sel;
        float m_cell_size = 1.0f;
    };

}
