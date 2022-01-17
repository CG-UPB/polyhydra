#pragma once

#include <string>
#include <vector>

namespace vOS
{
    class StringUtil
    {
    public:
        static std::vector<std::string> split_str(const std::string& string, const std::string& delimiter)
        {
            std::string s = string;
            std::vector<std::string> res;
            size_t pos;
            std::string token;
            while ((pos = s.find(delimiter)) != std::string::npos)
            {
                token = s.substr(0, pos);
                res.push_back(token);
                s.erase(0, pos + delimiter.length());
            }
            res.push_back(s);
            return res;
        }
    };
}