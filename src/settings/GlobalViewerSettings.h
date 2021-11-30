#pragma once

namespace vOS
{
    class GlobalViewerSettings
    {
    public:
        ~GlobalViewerSettings();
        static GlobalViewerSettings* getInstance();

    private:
        GlobalViewerSettings();
        static GlobalViewerSettings* instance;
    };

}
