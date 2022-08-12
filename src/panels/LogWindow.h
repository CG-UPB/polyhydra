#pragma once

#include "vospch.h"

#include "WindowPanel.h"

namespace volumeshOS::Internal
{

    class LogWindow final: public WindowPanel
    {
        public:

            void show() override;
            void add_message(std::string msg);

        private:

            std::vector<std::string> messages;

            static void show_message(const std::string& message);
    };
}