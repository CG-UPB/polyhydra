#pragma once

#include "vospch.h"

#include "WindowPanel.h"
#include "../settings/AppState.h"

namespace volumeshOS::Internal
{
    class QualityPanel final: public WindowPanel
    {
    public:

        /**
         * This method is used to draw the QualityPanel in every loop. It creates all the buttons and sets the corresponding
         * variables
         */
        void show() override;

    private:
        // display options
        const char* dropdown_presets[5] = {
                "OFF", "Quality", "Balanced", "Performance", "Custom"
        };

        static const int s_max_samples = 64;
    };
}