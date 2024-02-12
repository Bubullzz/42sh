#ifndef VARIABLES_H
#define VARIABLES_H

#include <stdlib.h>

struct user_var
{
    char *name;
    char *value;
    struct user_var *next;
};

struct special_var
{
    size_t argc;
    char **argv;
    size_t exit_code;
    size_t pid;
    size_t uid;
};

struct variables
{
    struct user_var *user_var;
    struct special_var *special_var;
};

struct variables *init_variables(char *argv[]);
void add_user_var(struct variables *variables, char *ass_word);
void free_variables(struct variables *variables);

void free_variables(struct variables *variables);

#endif /* ! VARIABLES_H */
