#pragma once

namespace volumeshOS::Internal
{
    /**
 *  Parent Class for any visible Windows inside the VosViewer such as the Logger, Toolbox and Mesh List
 */
    class WindowPanel
    {
    public:
        /**
         * Renders the Window Panel. Intended to be run each frame. Also responsible for Panel-internal logic
         */
        virtual void show() = 0;

        virtual ~WindowPanel() = default;
    };
} // namespace volumeshOS::Internal
