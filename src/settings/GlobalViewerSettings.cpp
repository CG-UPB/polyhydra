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
        /*bool new_selected_meshes[m_nbr_meshes+1];
        for (size_t i = 0; i < m_nbr_meshes; i++)
        {
            new_selected_meshes[i] = m_selected_mesh[i];
        }
        new_selected_meshes[m_nbr_meshes] = false;
*/
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