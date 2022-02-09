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


}