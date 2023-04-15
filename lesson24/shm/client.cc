#include "comm.hpp"
#include <unistd.h>

int main()
{
    // key_t k = GetKey();
    // cout << "client key: " << ToHex(k) << endl;

    // // 找到共享内存
    // int shmid = GetShm(k,gsize);
    // cout << "client shmid: " << shmid << endl;

    //  // 将自己和共享内存关联
    // char* start = AttachShm(shmid);

    // sleep(15);

    // // 将自己和共享内存去关联
    // DetachShm(start);

    Init init(CLIENT);
    char *start = init.getStart();

    char c = 'A';

    while(c <= 'Z')
    {
        start[c-'A'] = c;
        c++;
        start[c-'A'] = '\0';
        sleep(1);
    }

    return 0;
}