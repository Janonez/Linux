#pragma once

#include <iostream>
#include <string>

using namespace std;

class Task
{
public:
    Task()
    {}
    Task(int x,int y, char op)
        : _x(x)
        , _y(y)
        , _op(op)
        , _result(0)
        , _exitCode(0)
    {}
    void operator()()
    {
        switch(_op)
        {
            case '+':
                _result = _x + _y;
                break;
            case '-':
                _result = _x - _y;
                break; 
            case '*':
                _result = _x * _y;
                break;
            case '/':
            {
                if(_y != 0)
                    _result = _x / _y;
                else
                    _exitCode = -1;
            }
                break;
            case '%':
            {
                if(_y != 0)
                    _result = _x % _y;
                else
                    _exitCode = -2;
            }
                break;
            defalut :
                break;
        }
    }
    string formatArg()
    {
        return to_string(_x) + _op + to_string(_y) + '=';
    }
    string formatRes()
    {
        return to_string(_result) + "(" + to_string(_exitCode) + ")";
    }
    ~Task()
    {}
private:
    int _x;
    int _y;
    char _op;

    int _result;
    int _exitCode;
};