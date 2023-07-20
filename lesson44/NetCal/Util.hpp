#pragma once

#include <iostream>
#include <string>
#include <vector>

class Util
{
public:
    // 输入：const &
    // 输出：*
    // 输入输出：&
    static bool StringSplit(const std::string& str,const std::string& sep, std::vector<std::string>* result)
    {
        // 10 + 20
        size_t start = 0;
        
        while(start < str.size())
        {
            auto pos = str.find(sep, start);
            if(pos == std::string::npos)
                break;
            
            result->push_back(str.substr(start,pos-start));
            start = pos + sep.size();// 更新位置
        }
        if(start < str.size())
        {
            result->push_back(str.substr(start));
        }
        return true;
    }
    static int toInt(const std::string &s)
    {
        return atoi(s.c_str());
    }
};