#include "9cc.h"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        // error("引数の個数が正しくねぇ！！！");
        return 1;
    }

    char *user_input = argv[1];

    return 0;
}