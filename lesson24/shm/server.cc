#include "comm.hpp"
#include <unistd.h>

int main()
{
    // // 创建key
    // key_t k = GetKey();
    // cout << "server key: " << ToHex(k) << endl;

    // // 创建共享内存
    // int shmid = CreatShm(k,gsize);
    // cout << "server shmid: " << shmid << endl;

    // sleep(3);

    // // 将自己和共享内存关联
    // char* start = AttachShm(shmid);

    // sleep(20);

    // // 将自己和共享内存去关联
    // DetachShm(start);

    // // struct shmid_ds ds;
    // // int n = shmctl(shmid,IPC_STAT,&ds);
    // // if(n != -1)
    // // {
    // //     cout << "key: " << ToHex(ds.shm_perm.__key) << endl;
    // //     cout << "creater pid: " << ds.shm_cpid << " : " << getpid() << endl;
    // // }

    // // 删除共享内存
    // DelShm(shmid);

    Init init(SERVER);
    char *start = init.getStart();

    int n = 0;
    while(n <= 30)
    {
        cout <<"client -> server# "<< start << endl;
        sleep(1);
        n++;
    }
    return 0;
}