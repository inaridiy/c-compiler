#include "9cc.h"

Token *token;
char *user_input;

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        // error("引数の個数が正しくねぇ！！！");
        return 1;
    }

    user_input = argv[1];
    token = tokenize(user_input);

    return 0;
}