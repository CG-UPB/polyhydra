#include "GlobalViewerSettings.h"

namespace vOS
{
    GlobalViewerSettings* GlobalViewerSettings::instance = 0;

    // Singleton
    GlobalViewerSettings* GlobalViewerSettings::getInstance()
    {
        if (instance == 0)
        {
            instance = new GlobalViewerSettings();
        }

        return instance;

    }

    // Destruktor
    GlobalViewerSettings::~GlobalViewerSettings()
    {
        //delete instance;
    }

    GlobalViewerSettings::GlobalViewerSettings() {
        //m_sel.push_back(false);
    };

    void GlobalViewerSettings::m_new_Mesh()
    {
        m_sel.push_back(true);
        m_nbr_meshes++;
        
    }

    bool GlobalViewerSettings::get_Visibility_of_Mesh(int i) {
        return m_sel[i];
    }

    void GlobalViewerSettings::set_Visibility_of_Mesh(int i, bool b) {
        m_sel[i] = b;
    }
}