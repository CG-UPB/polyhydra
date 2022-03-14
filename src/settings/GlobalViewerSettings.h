#pragma once

#include "../panels/ToolBar.h"
#include "../mesh/MeshObject.h"
#include "../util/ModeEnum.h"

namespace vOS
{

    /**
     * Use this to configure the ssao effect.
     */
    struct SSAOOptions
    {
        bool active         = true;
        int num_samples     = 64;
        float sample_radius = 0.5;
        float strength      = 1.5;
        float z_bias        = 0.01;
    };

    class GlobalViewerSettings
    {
    public:
        static GlobalViewerSettings* getInstance();

        void set_selection_activated(bool activated){ m_current_selection_activated = activated;};
        void set_selection_mode(int mode){ m_current_selection_mode = mode;};
        void set_isolation_state(bool isolation_state){ m_isolation_started = isolation_state;};
        void set_mesh_mode(int mode){ mesh_mode = mode;};
        void set_digging_active(bool b){ m_current_digging_active = b;};
        void set_rounding_active(bool r){ m_rounding_activated = r;};
        void set_rounding_size(float r){ m_rounding_size = r;};
        void set_transparency_mode(int t){ m_current_transparency_mode = t;};
        void set_number_passes(int np){ m_current_number_passes = np;};
        void set_transparency_activated(bool t){ transparency_active = t;};
        void set_shadows_activated(bool s){ shadows_active = s;};
        void set_ambient_occlusion_activated(bool ao){ ambient_occlusion_active = ao;};
        void set_ssao_options(int s){ ssao_option = s;};
        void set_custom_options(const SSAOOptions& s){ current_custom_options = s;};
        void set_wireframe_size(float size) { m_wireframe_size = size; };
        void set_vertex_only_size(float size) { m_vertex_only_size = size; };

        [[nodiscard]] bool get_selection_activated() const{return m_current_selection_activated;};
        [[nodiscard]] bool get_digging_activated() const{return m_current_digging_active;};
        [[nodiscard]] int get_selection_mode() const{return m_current_selection_mode;};
        [[nodiscard]] bool get_isolation_state() const{return m_isolation_started;};
        [[nodiscard]] int get_mesh_mode() const{return mesh_mode;};
        [[nodiscard]] bool get_rounding_active() const{return m_rounding_activated;};
        [[nodiscard]] float get_rounding_size() const{return m_rounding_size;};
        [[nodiscard]] int get_transparency_mode() const{return m_current_transparency_mode;};
        [[nodiscard]] int get_number_passes() const{return m_current_number_passes;};
        [[nodiscard]] bool get_transparency_activated() const{return transparency_active;};
        [[nodiscard]] bool get_shadows_activated() const{return shadows_active;};
        [[nodiscard]] bool get_ambient_occlusion_activated() const{return ambient_occlusion_active;};
        [[nodiscard]] int get_ssao_options() const{return ssao_option;};
        [[nodiscard]] SSAOOptions get_custom_options() const{return current_custom_options;};
        [[nodiscard]] float get_wireframe_size() const{return m_wireframe_size;};
        [[nodiscard]] float get_vertex_only_size() const{return m_vertex_only_size;};

    private:
        GlobalViewerSettings();
        static GlobalViewerSettings* instance;
        bool m_current_selection_activated = false;
        int m_current_selection_mode = 0;
        bool m_isolation_started = false;
        bool m_current_digging_active = false;
        int mesh_mode = Phong_Facenormals;
        float m_rounding_size = 0.3f;
        int m_current_transparency_mode = WEIGHTED_BLENDED;
        int m_current_number_passes = 12;
        int m_current_cascade_level = 1;

        // booleans for the render passes
        bool transparency_active = false;
        bool shadows_active = false;
        bool ambient_occlusion_active = false;
        bool m_rounding_activated = false;
        float m_wireframe_size = 0.35f;
        float m_vertex_only_size = 0.4f;

        // AO
        int ssao_option = QUALITY;
        SSAOOptions current_custom_options;
    };

}
