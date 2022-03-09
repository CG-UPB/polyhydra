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
        ~GlobalViewerSettings();
        static GlobalViewerSettings* getInstance();

        void m_set_current_selection_activated(bool activated){m_current_selection_activated = activated;};
        void m_set_current_selection_mode(int mode){m_current_selection_mode = mode;};
        void m_set_current_isolation_state(bool isolation_state){m_isolation_started = isolation_state;};
        void m_set_current_mesh_mode(int mode){mesh_mode = mode;};
        void m_set_current_digging_active(bool b){m_current_digging_active = b;};
        void m_set_current_rounding_active(bool r){m_rounding_activated = r;};
        void m_set_current_rounding_size(float r){m_rounding_size = r;};
        void m_set_current_transparency_mode(int t){m_current_transparency_mode = t;};
        void m_set_current_number_passes(int np){m_current_number_passes = np;};
        void m_set_current_transparency_activated(bool t){transparency_active = t;};
        void m_set_current_shadows_activated(bool s){shadows_active = s;};
        void m_set_current_ambient_occlusion_activated(bool ao){ambient_occlusion_active = ao;};
        void m_set_current_ssao_option(int s){ssao_option = s;};
        void m_set_current_costume_options(const SSAOOptions& s){current_costume_options = s;};
        void m_set_current_costume_options(SSAOOptions* s){ current_custom_options = s;};

        [[nodiscard]] bool m_get_current_selection_activated() const{return m_current_selection_activated;};
        [[nodiscard]] bool m_get_current_digging_activated() const{return m_current_digging_active;};
        [[nodiscard]] int m_get_current_selection_mode() const{return m_current_selection_mode;};
        [[nodiscard]] bool m_get_current_isolation_state() const{return m_isolation_started;};
        [[nodiscard]] int m_get_current_mesh_mode() const{return mesh_mode;};
        [[nodiscard]] bool m_get_current_rounding_active() const{return m_rounding_activated;};
        [[nodiscard]] float m_get_current_rounding_size() const{return m_rounding_size;};
        [[nodiscard]] int m_get_current_transparency_mode() const{return m_current_transparency_mode;};
        [[nodiscard]] int m_get_current_number_passes() const{return m_current_number_passes;};
        [[nodiscard]] bool m_get_current_transparency_activated() const{return transparency_active;};
        [[nodiscard]] bool m_get_current_shadows_activated() const{return shadows_active;};
        [[nodiscard]] bool m_get_current_ambient_occlusion_activated() const{return ambient_occlusion_active;};
        [[nodiscard]] int m_get_current_ssao_option() const{return ssao_option;};
        [[nodiscard]] SSAOOptions m_get_current_costume_options() const{return current_costume_options;};
        [[nodiscard]] SSAOOptions* m_get_current_costume_options() const{return current_custom_options;};



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

        // booleans for the render passes
        bool transparency_active = false;
        bool shadows_active = false;
        bool ambient_occlusion_active = false;
        bool m_rounding_activated = false;

        // AO
        int ssao_option = QUALITY;
        SSAOOptions *current_custom_options = new SSAOOptions();
    };

}
