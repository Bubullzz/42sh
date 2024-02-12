#define _POSIX_C_SOURCE 200112L
#include "variables.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static struct special_var *init_special_var(char *argv[])
{
    struct special_var *res = calloc(1, sizeof(struct special_var));
    if (res == NULL)
    {
        fprintf(stderr, "init_special_var: calloc failed\n");
        return NULL;
    }

    res->argv = argv + 1;
    char **temp = argv + 1;
    while (temp[res->argc] != NULL)
    {
        res->argc++;
    }
    res->exit_code = 0;
    res->pid = getpid();
    res->uid = getuid();

    return res;
}

struct variables *init_variables(char *argv[])
{
    struct variables *res = calloc(1, sizeof(struct variables));
    if (res == NULL)
    {
        fprintf(stderr, "init_variables: calloc failed\n");
        return NULL;
    }
    res->user_var = NULL;

    res->special_var = init_special_var(argv);
    if (res->special_var == NULL)
    {
        return NULL;
    }

    return res;
}

void add_user_var(struct variables *variables, char *ass_word)
{
    char *name = NULL;
    char *value = NULL;

    int i = 0;
    while (ass_word[i] != '=')
    {
        i++;
    }
    name = calloc(i + 1, sizeof(char));
    name = memcpy(name, ass_word, i);

    value = calloc(strlen(ass_word) - i, sizeof(char));
    value = strcpy(value, ass_word + i + 1);

    // debut des modifs
    if (getenv(name))
    {
        setenv(name, value, 1);
        free(name);
        free(value);
        return;
    }
    // fin des modifs

    struct user_var *temp = variables->user_var;

    while (temp != NULL)
    {
        if (strcmp(temp->name, name) == 0)
        {
            free(temp->value);
            temp->value = value;

            free(name);
            return;
        }

        temp = temp->next;
    }

    // It is a new variable
    struct user_var *new = calloc(1, sizeof(struct user_var));
    new->next = variables->user_var;
    new->name = name;
    new->value = value;

    variables->user_var = new;
}

static void free_user_var(struct user_var *user_var)
{
    if (user_var != NULL)
    {
        free_user_var(user_var->next);

        free(user_var->name);
        free(user_var->value);
        free(user_var);
    }
}

void free_variables(struct variables *variables)
{
    free_user_var(variables->user_var);

    free(variables->special_var);

    free(variables);
}
