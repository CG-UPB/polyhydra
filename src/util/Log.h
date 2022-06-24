#pragma once

#include <string>
#include <iostream>

namespace volumeshOS
{
    class Log
    {
        enum class Level
        {
            INFO = 0,
            WARNING = 1,
            ERROR = 2
        };

    public:

        static inline void set_level(Level level)
        {
            s_log_level = level;
        }

        template<typename T>
        static inline void info(T message)
        {
            if (s_log_level <= Level::INFO)
            {
                log(Level::INFO, message);
            }
        }

        template<typename T>
        static inline void warn(T message)
        {
            if (s_log_level <= Level::WARNING)
            {
                log(Level::WARNING, message);
            }
        }

        template<typename T>
        static inline void error(T message)
        {
            if (s_log_level <= Level::ERROR)
            {
                log(Level::ERROR, message);
            }
        }

    private:

        template<typename T>
        static inline void log(Level level, T message)
        {
            switch (level)
            {
                case Level::INFO:
                    std::cout << ANSI_BOLD << PREFIX << " " << PREFIX_TYPE_INFO << ": ";
                    std::cout << ANSI_RESET << message << std::endl;
                    break;
                case Level::WARNING:
                    std::cout << ANSI_BOLD_YELLOW << PREFIX << " " << PREFIX_TYPE_WARN << ": ";
                    std::cout << ANSI_RESET << ANSI_YELLOW << message << ANSI_RESET << std::endl;
                    break;
                case Level::ERROR:
                    std::cout << ANSI_BOLD_RED << PREFIX << " " << PREFIX_TYPE_ERROR << ": ";
                    std::cout << ANSI_RESET << ANSI_RED << message << ANSI_RESET << std::endl;
                    break;
            }
        }

        static Level s_log_level;

        // Prefix
        static constexpr const char* PREFIX             = "[volumeshOS]";
        static constexpr const char* PREFIX_TYPE_INFO   = "[info]";
        static constexpr const char* PREFIX_TYPE_WARN   = "[warn]";
        static constexpr const char* PREFIX_TYPE_ERROR  = "[error]";

        // ANSI codes
        static constexpr const char* ANSI_RESET         = "\033[0m";
        static constexpr const char* ANSI_BOLD          = "\033[1m";
        static constexpr const char* ANSI_RED           = "\033[31m";
        static constexpr const char* ANSI_YELLOW        = "\033[33m";
        static constexpr const char* ANSI_BOLD_RED      = "\033[1m\033[31m";
        static constexpr const char* ANSI_BOLD_YELLOW   = "\033[1m\033[33m";
    };
}
