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

    GlobalViewerSettings::GlobalViewerSettings() {}

    // Destruktor
    GlobalViewerSettings::~GlobalViewerSettings()
    {
        //delete instance;
    }
}