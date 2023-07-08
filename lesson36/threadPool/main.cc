//#include "ThreadPool_V1.hpp"
#include "ThreadPool_V2.hpp"
#include "Task.hpp"
#include <memory>

int main()
{
    std::unique_ptr<ThreadPool<Task>> tp(new ThreadPool<Task>());
    tp->init();
    tp->start();
    tp->check();
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

        tp->pushTask(t);// 将任务传递到线程池中执行

        // 充当生产者，从网络中读取数据，构建为任务，推送给线程池
        //sleep(1);
        //tp->pushTask();
    }
    return 0;
}