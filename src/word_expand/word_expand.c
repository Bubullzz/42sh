#include "word_expand.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "handle_dollar.h"
#include "variables.h"

static bool is_in(char *delim, char c)
{
    size_t i = 0;
    while (delim[i] && delim[i] != c)
    {
        i++;
    }
    return delim[i];
}

static struct char_context *create_context(char *input)
{
    struct char_context *context = calloc(1, sizeof(struct char_context));
    if (context == NULL)
    {
        fprintf(stderr, "create_context: Calloc failed");
        return NULL;
    }
    context->buffer_len = strlen(input) * 3 + 1024;
    context->buffer = calloc(context->buffer_len, sizeof(char));
    if (context->buffer == NULL)
    {
        fprintf(stderr, "create_context: Calloc failed");
        return NULL;
    }

    context->input_split = calloc(2, sizeof(char *));
    if (context->input_split == NULL)
    {
        fprintf(stderr, "create_context: Calloc failed");
        return NULL;
    }

    return context;
}

// This method will expand every dollar symbol possible
static void fill_buffer(char *word, struct char_context **context,
                        struct variables *variables)
{
    size_t i = 0;
    while (word[i] != 0)
    {
        switch (word[i])
        {
        case '\'':
            if (!(*context)->escaped)
            {
                (*context)->single_quoted = 1 - (*context)->single_quoted;
            }
            (*context)->escaped = false;
            break;
        case '"':
            if (!(*context)->single_quoted && (*context)->escaped)
            {
                (*context)->double_quoted = 1 - (*context)->double_quoted;
            }
            (*context)->escaped = false;
            break;
        case '\\':
            (*context)->escaped = 1 - (*context)->escaped;
            break;
        case '$':
            if (!(*context)->single_quoted && !(*context)->escaped)
            {
                i++;
                handle_dollar(word, &i, context, variables);
                continue;
            }
            (*context)->escaped = false;
            break;
        default:
            (*context)->escaped = false;
            break;
        }

        (*context)->buffer[(*context)->word_len++] = word[i++];
    }
}

static char char_squote(char *input, size_t *i, struct char_context **context)
{
    if (input[*i] == '\'')
    {
        (*context)->single_quoted = false;
        return 0;
    }
    return input[*i];
}

static char char_dquote(char *input, size_t *i, struct char_context **context)
{
    if ((*context)->escaped)
    {
        (*context)->escaped = false;
        if (input[*i] == '\n')
        {
            (*context)->cur_word_len -= 2;
            return (*context)
                ->input_split[(*context)->cur_word][(*context)->cur_word_len];
        }
    }
    else
    {
        if (input[*i] == '"')
        {
            (*context)->double_quoted = false;
            return 0;
        }
        else if (input[*i] == '\\' && is_in("$`\"\\\n", input[(*i) + 1]))
        {
            (*context)->escaped = true;
            return 0;
        }
    }
    return input[*i];
}

static char char_normal(char *input, size_t *i, struct char_context **context)
{
    if ((*context)->escaped)
    {
        (*context)->escaped = false;
        if (input[*i] == '\n')
        {
            (*context)->cur_word_len -= 2;
            return (*context)
                ->input_split[(*context)->cur_word][(*context)->cur_word_len];
        }
        return input[*i];
    }

    char *ifs = " ";
    if (is_in(ifs, input[*i]))
    {
        (*i)++;
        if ((*context)->cur_word_len == 0)
        {
            free((*context)->input_split[(*context)->cur_word]);
            (*context)->input_split[(*context)->cur_word] = NULL;
            (*context)->cur_word--;
        }
        else
        {
            (*context)->input_split[(*context)->cur_word] =
                realloc((*context)->input_split[(*context)->cur_word],
                        (*context)->cur_word_len + 1);
        }
        (*context)->cur_word++;
        (*context)->input_split =
            realloc((*context)->input_split,
                    ((*context)->cur_word + 2) * sizeof(char *));
        (*context)->input_split[(*context)->cur_word] =
            calloc((*context)->buffer_len, sizeof(char));
        (*context)->input_split[(*context)->cur_word + 1] = NULL;
        (*context)->cur_word_len = 0;
        while (is_in(ifs, input[*i]))
        {
            (*i)++;
        }
        (*i)--;
        return 0;
    }
    else if (input[*i] == '\\')
    {
        (*context)->escaped = true;
        return 0;
    }
    else if (input[*i] == '\'')
    {
        (*context)->single_quoted = true;
        return 0;
    }
    else if (input[*i] == '"')
    {
        (*context)->double_quoted = true;
        return 0;
    }

    return input[*i];
}

static void get_words(char *input, struct char_context **context)
{
    size_t i = 0;
    (*context)->input_split[0] = calloc((*context)->buffer_len, sizeof(char));
    while (input[i] != 0)
    {
        char c = 0;
        if ((*context)->single_quoted)
        {
            c = char_squote(input, &i, context);
        }
        else if ((*context)->double_quoted)
        {
            c = char_dquote(input, &i, context);
        }
        else
        {
            c = char_normal(input, &i, context);
        }

        if (c != 0)
        {
            (*context)->input_split[(*context)->cur_word]
                                   [(*context)->cur_word_len++] = c;
        }
        i++;
    }
}

char **word_expand(char *word, struct variables *variables)
{
    struct char_context *context = create_context(word);
    if (context == NULL)
    {
        return NULL;
    }

    fill_buffer(word, &context, variables);
    context->double_quoted = false;
    context->single_quoted = false;
    context->escaped = false;

    get_words(context->buffer, &context);
    if (context->cur_word_len == 0)
    {
        free(context->input_split[context->cur_word]);
        context->input_split[context->cur_word] = NULL;
    }

    char **res = context->input_split;

    free(context->buffer);
    free(context);
    return res;
}

void free_expanded(char **expanded)
{
    for (size_t i = 0; expanded[i] != 0; i++)
    {
        free(expanded[i]);
    }
    free(expanded);
}
