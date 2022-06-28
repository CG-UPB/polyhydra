#include "GlobalViewerSettings.h"

namespace volumeshOS::Internal
{
    std::shared_ptr<GlobalViewerSettings> GlobalViewerSettings::instance = nullptr;

    // Singleton
    std::shared_ptr<GlobalViewerSettings> GlobalViewerSettings::getInstance()
    {
        if (instance == nullptr)
        {
            instance = std::make_shared<GlobalViewerSettings>();
        }
        return instance;
    }
}