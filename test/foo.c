#include <stdio.h>

int foo()
{
    printf("OK\n");
    return 123;
}

int bar(int a, int b)
{
    printf("bar\n");
    printf("a = %d\n", a);
    printf("b = %d\n", b);
    return a * b;
}