#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include "Util.hpp"

namespace protocol_ns
{
    #define SEP " "
    #define SEP_LEN strlen(SEP) // 绝对不能写成sizeof
    #define HEADER_SEP "\r\n"
    #define HEADER_SEP_LEN strlen("\r\n")
    // "长度"\r\n"_x _op _y"\r\n
    // "10 + 20" ==> "7"\r\n"10 + 20"\r\n ==> 报头 + 有效载荷
    // 请求/响应 = 报头\r\n有效载荷\r\n

    // "10 + 20" ==> "7"\r\n"10 + 20"\r\n 
    void AddHeader(const std::string& str)
    {
        std::string s = std::to_string(str.size());
        s += HEADER_SEP;
        s += str;
        s += HEADER_SEP;
    }
    // "7"\r\n""10 + 20"\r\n => "10 + 20"
    void RemoveHeader()
    {
        
    }

    // 给网络版本计算机定制协议
    class Request
    {
    public:
        Request()
        {
        }
        Request(int x, int y, char op)
            : _x(x), _y(y), _op(op)
        {
        }
        // 序列化 struct->string
        // _x _op _y
        bool Serialize(std::string *outStr)
        {
            *outStr = "";
            std::string x_string = std::to_string(_x);
            std::string y_string = std::to_string(_y);

            // 手动序列化
            *outStr = x_string + SEP + _op + SEP + y_string;

            return true;
        }
        // 反序列化 string->struct
        bool DeSerialize(const std::string &inStr)
        {
            // inStr ：10 + 20 => [0]=>10, [1]=>+, [2]=>20
            std::vector<std::string> result;
            Util::StringSplit(inStr, SEP, &result);

            if(result.size() != 3)
                return false;
            if(result[1].size() != 1)
                return false;
            
            _x = Util::toInt(result[0]);
            _op = result[1][0];
            _y = Util::toInt(result[2]);
            
            return true;
        }
        ~Request()
        {
        }

    public:
        int _x;
        int _y;
        char _op;
    };

    class Response
    {
    public:
        Response()
        {
        }
        Response(int result, int code)
            : _result(result), _code(code)
        {
        }
        // 序列化 struct->string
        bool Serialize(std::string *outStr)
        {
            // _result _code
            *outStr = "";
            std::string res_string = std::to_string(_result);
            std::string code_string = std::to_string(_code);

            *outStr = res_string + SEP + code_string;
            return true;
        }
        // 反序列化 string->struct
        bool DeSerialize(const std::string &inStr)
        {
            // 10 0, 10 1
            std::vector<std::string> result;
            Util::StringSplit(inStr, SEP, &result);

            if(result.size() != 2)
                return false;
            
            _result = Util::toInt(result[0]);
            _code = Util::toInt(result[1]);
            
            return true;
        }
        ~Response()
        {
        }

    public:
        int _result;
        int _code; // 0 success , 1,2,3,4代表不同的错误码
    };
}
