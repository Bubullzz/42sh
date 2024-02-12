#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "command.h"

int builtin_exit(char *code)
{
    if (!code)
        exit(0);
    int ret = 0;
    size_t i = 0;
    while (code[i] && code[i] < '9' && code[i] > '0')
    {
        ret *= 10;
        ret += (code[i] - '0');
        i++;
    }
    if (code[i])
    {
        fprintf(stderr, "exit: need an integer as parameter\n");
        exit(2);
    }
    exit(ret % 256);
}
