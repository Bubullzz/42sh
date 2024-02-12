#include "handle_dollar.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void expand_args(struct char_context **context,
                        struct variables *variables)
{
    char **temp = variables->special_var->argv;
    if (temp == NULL)
    {
        (*context)->word_len++;
    }
    else
    {
        size_t i = 0;
        while (temp[i] != NULL)
        {
            size_t j = 0;
            while (temp[i][j] != 0)
            {
                (*context)->buffer[(*context)->word_len] = temp[i][j];
                j++;
                (*context)->word_len++;
            }
            (*context)->buffer[(*context)->word_len] = ' ';
            (*context)->word_len++;
            i++;
        }
        (*context)->word_len--;
        (*context)->buffer[(*context)->word_len] = 0;
    }
}

static bool is_special(char *input, size_t *i, struct char_context **context,
                       struct variables *variables)
{
    char c = input[(*i)++];
    if (c == '@' || c == '*')
    {
        expand_args(context, variables);
    }
    else if (c == '?')
    {
        (*context)->word_len +=
            sprintf((*context)->buffer + (*context)->word_len, "%ld",
                    variables->special_var->exit_code);
    }
    else if (c == '$')
    {
        (*context)->word_len +=
            sprintf((*context)->buffer + (*context)->word_len, "%ld",
                    variables->special_var->pid);
    }
    else if (c == '#')
    {
        (*context)->word_len +=
            sprintf((*context)->buffer + (*context)->word_len, "%ld",
                    variables->special_var->argc);
    }
    else
    {
        (*i)--;
        return false;
    }

    return true;
}

static char *expand_var_rec(char **var_exp)
{
    size_t final_len = 0;
    for (size_t i = 0; var_exp[i] != NULL; i++)
    {
        final_len += strlen(var_exp[i]) + 1;
    }

    char *oui = calloc(final_len + 1, sizeof(char));
    char *oui2 = oui;

    for (size_t i = 0; var_exp[i] != NULL; i++)
    {
        oui2 = memcpy(oui2, var_exp[i], strlen(var_exp[i]));
        oui2 += strlen(var_exp[i]);
        (*oui2) = ' ';
        oui2 += 1;
    }

    *(oui2 - 1) = 0;
    return oui;
}

static void put_var_val(char *name, struct char_context **context,
                        struct variables *variables)
{
    if (strcmp(name, "RANDOM") == 0)
    {
        (*context)->word_len +=
            sprintf((*context)->buffer + (*context)->word_len, "%u", rand());
    }
    else if (strcmp(name, "UID") == 0)
    {
        (*context)->word_len +=
            sprintf((*context)->buffer + (*context)->word_len, "%ld",
                    variables->special_var->uid);
    }
    else if (strcmp(name, "OLDPWD") == 0)
    {
        (*context)->word_len += sprintf(
            (*context)->buffer + (*context)->word_len, "%s", getenv("OLDPWD"));
    }
    else if (strcmp(name, "PWD") == 0)
    {
        (*context)->word_len += sprintf(
            (*context)->buffer + (*context)->word_len, "%s", getenv("PWD"));
    }
    else if (strcmp(name, "IFS") == 0)
    {
        (*context)->word_len += sprintf(
            (*context)->buffer + (*context)->word_len, "%s", getenv("IFS"));
    }
    else
    {
        // debut des modifs
        if (getenv(name))
        {
            free((*context)->buffer);
            char *val = getenv(name);
            size_t len = strlen(val) + 1;
            char *cpy = malloc(len);
            cpy = strncpy(cpy, val, len);
            (*context)->buffer = cpy;
            (*context)->buffer_len = len;
            return;
        }
        // fin des modifs
        struct user_var *temp = variables->user_var;
        while (temp && strcmp(temp->name, name) != 0)
        {
            temp = temp->next;
        }
        if (temp != NULL)
        {
            char **var_exp = word_expand(temp->value, NULL);
            char *oui = expand_var_rec(var_exp);
            (*context)->word_len +=
                sprintf((*context)->buffer + (*context)->word_len, "%s", oui);
            free_expanded(var_exp);
            free(oui);
        }
    }
}

static bool is_param_exp(char *input, size_t *i, struct char_context **context,
                         struct variables *variables)
{
    bool bracket = false;
    if (input[*i] == '{')
    {
        (*i)++;
        bracket = true;
    }

    size_t name_len = 0;
    if ((input[*i] >= 'A' && input[*i] <= 'Z')
        || (input[*i] >= 'a' && input[*i] <= 'z') || input[*i] == '_')
    {
        name_len++;
        while ((input[*i + name_len] >= 'A' && input[*i + name_len] <= 'Z')
               || (input[*i + name_len] >= 'a' && input[*i + name_len] <= 'z')
               || (input[*i + name_len] >= '0' && input[*i + name_len] <= '9')
               || input[*i + name_len] == '_')
        {
            name_len++;
        }
    }

    if (is_special(input, i, context, variables))
    {
        if (bracket)
        {
            if (input[*i] != '}')
            {
                fprintf(stderr, "is_param_exp: Bad substitution");
                (*context)->error = true;
            }
            (*i)++;
        }
        return true;
    }
    else if (name_len != 0)
    {
        char *name = calloc(name_len + 1, sizeof(char));
        name = memcpy(name, input + (*i), name_len);
        put_var_val(name, context, variables);
        free(name);
        (*i) += name_len;
        if (bracket)
        {
            if (input[*i] != '}')
            {
                fprintf(stderr, "is_param_exp: Bad substitution");
                (*context)->error = true;
            }
            (*i)++;
        }
        return true;
    }
    return false;
}

void handle_dollar(char *input, size_t *i, struct char_context **context,
                   struct variables *variables)
{
    // test for arithmetic expansion
    // test for command subsitution
    if (is_param_exp(input, i, context, variables))
    {
        return;
    }
}
