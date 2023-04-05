#include <stdio.h>

#define ONE 0x1
#define TWO 0x2
#define THREE 0x4
#define FOUR 0x8
#define FIVE 0x10

// 0000 0000 0000 0000 0000 0000 0000 0000
void Print(int flags)
{
    if(flags & ONE) printf("hello 1\n"); //充当不同的行为
    if(flags & TWO) printf("hello 2\n");
    if(flags & THREE) printf("hello 3\n");
    if(flags & FOUR) printf("hello 4\n");
    if(flags & FIVE) printf("hello 5\n");
}


int main()
{
    printf("--------------------------\n");
    Print(ONE);
    printf("--------------------------\n");
    Print(TWO);
    printf("--------------------------\n");
    Print(FOUR);
    printf("--------------------------\n");

    Print(ONE|TWO);
    printf("--------------------------\n");

    Print(ONE|TWO|THREE);
    printf("--------------------------\n");

    Print(ONE|TWO|THREE|FOUR|FIVE);
    printf("--------------------------\n");

    return 0;
}
