#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "builtins.h"

static int is_readonly(char *name)
{
    char *readonly[] = { "@", "*", "?", "$", "1", "#", "RANDOM", "UID", NULL };
    size_t i = 0;
    while (readonly[i] && strcmp(readonly[i++], name))
        ;
    return readonly[i] != NULL;
}

int builtin_export(char **args)
{
    if (!(*args))
        return 1;
    char *name = strtok(args[0], "=\n");
    char *value = strtok(NULL, "\n");
    int r = 1;
    if (!is_readonly(name))
    {
        r = setenv(name, (value ? value : ""), 1);
    }
    else
    {
        fprintf(stderr, "export: cannot export readonly values\n");
    }
    return r;
}

int builtin_unset(char **args)
{
    if (!args)
        return 1;
    return 0;
}
