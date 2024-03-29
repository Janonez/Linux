// #include "ThreadPool_V1.hpp"
// #include "ThreadPool_V2.hpp"
// #include "ThreadPool_V3.hpp"
#include "ThreadPool_V4.hpp"
#include "Task.hpp"
#include <memory>

int main()
{
    while(1)
    {
        //--------------------选择输入要计算的数字和方式----------------------------------
        int x,y;
        char op;
        std::cout << "please Enter x > ";
        std::cin >> x;

        std::cout << "please Enter y > ";
        std::cin >> y;
        
        std::cout << "please Enter op(+-*/%) > ";
        std::cin >> op;
        //-------------------------------------------------------------------------------
        
        Task t(x,y,op); // 构建任务
        ThreadPool<Task>::getinstance()->pushTask(t);

    }
    return 0;
}