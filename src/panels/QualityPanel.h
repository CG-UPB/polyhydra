#pragma once

#include "vospch.h"

#include "../ImguiRenderer.h"
#include "../settings/GlobalViewerSettings.h"

namespace vOS
{
    class QualityPanel final: public WindowPanel
    {
    public:
        /**
         * creates an instance of the QualityPanel
         */
        QualityPanel();
        ~QualityPanel();
        /**
         * This method is used to draw the QualityPanel in every loop. It creates all the buttons and sets the corresponding
         * variables
         */
        void show() override;

    private:
        // display options
        const char* dropdown_presets[5] = {
                "Off", "Quality", "Balanced", "Performance", "Custom"
        };
        // index of a particular option
        static const int s_max_samples = 64;
        static const int s_noise_size = 4;

    };
}