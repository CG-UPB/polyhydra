#pragma once

#include "../panels/ToolBar.h"
#include "../mesh/MeshObject.h"
#include "../util/ModeEnum.h"

namespace vOS
{
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

        [[nodiscard]] bool m_get_current_selection_activated() const{return m_current_selection_activated;};
        [[nodiscard]] bool m_get_current_digging_activated() const{return m_current_digging_active;};
        [[nodiscard]] int m_get_current_selection_mode() const{return m_current_selection_mode;};
        [[nodiscard]] int m_get_current_mesh_mode() const{return mesh_mode;};



    private:
        GlobalViewerSettings();
        static GlobalViewerSettings* instance;
        bool m_current_selection_activated = false;
        int m_current_selection_mode = 0;
        bool m_isolation_started = false;
        bool m_current_digging_active = false;
        int mesh_mode = 0;
    };

}
