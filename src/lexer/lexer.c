#include "lexer.h"

#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "grammar_rules.h"

static bool is_in(char *delim, char c)
{
    size_t i = 0;
    while (delim[i] && delim[i] != c)
    {
        i++;
    }
    return delim[i];
}

static char *build_string(char *buffer, size_t len)
{
    char *word = malloc(len + 1);
    strcpy(word, buffer);
    return word;
}

static struct lexer_context *create_context(char *input)
{
    struct lexer_context *context = calloc(1, sizeof(struct lexer_context));
    if (!context)
    {
        fprintf(stderr, "create_context: Cannot calloc!\n");
        return NULL;
    }
    context->single_quoted = false;
    context->double_quoted = false;
    context->escaped = false;

    char *buffer = calloc(strlen(input) + 1, sizeof(char));
    if (!buffer)
    {
        fprintf(stderr, "create_context: Cannot calloc!\n");
        return NULL;
    }
    context->buffer = buffer;

    return context;
}

static struct token *add_eof_token(struct token *token)
{
    struct token *new = calloc(1, sizeof(struct token));
    if (!new)
    {
        fprintf(stderr, "add_of_token: Cannot calloc!\n");
        return NULL;
    }

    struct token *temp = token;
    while (temp->next)
    {
        temp = temp->next;
    }
    temp->next = new;
    new->type = LEXER_EOF;
    new->value = VAL_EOF;
    return token;
}

static void set_context_type(char c, struct lexer_context **context)
{
    char *operators = "\n|&;<>";
    if (is_in(operators, c))
    {
        (*context)->type = CONTEXT_OPE;
    }
    else if (c >= '0' && c <= '9')
    {
        (*context)->type = CONTEXT_NUM_POSSIBLE;
    }
    else
    {
        (*context)->type = CONTEXT_TOKEN;
    }
}

static enum token_type set_token_type(struct lexer_context *context)
{
    switch (context->type)
    {
    case CONTEXT_NUM:
        return LEXER_IO_NUMBER;
    case CONTEXT_OPE:
        return LEXER_OPERATOR;
    default:
        return LEXER_WORD;
    }
}

static bool is_operator(char c, char *buffer, size_t len)
{
    char *operators[] = { "\n", "|",  "&",  ";",  "<",  ">",   "&&", "||", ";;",
                          "<<", ">>", ">&", "<&", "<>", "<<-", ">|", NULL };

    buffer[len] = c;
    size_t j = 0;
    while (operators[j] && strcmp(operators[j], buffer))
    {
        j++;
    }
    buffer[len] = 0;
    return operators[j] != NULL;
}

static char handle_char(char *input, size_t *i, struct lexer_context **context)
{
    if ((*context)->type == CONTEXT_NEW)
    {
        set_context_type(input[*i], context);
    }

    if ((*context)->type == CONTEXT_OPE)
    {
        if (!is_operator(input[*i], (*context)->buffer, (*context)->token_len))
        {
            return 0;
        }
    }
    else
    {
        char *temp = calloc(2, sizeof(char));
        if (is_operator(input[*i], temp, 0))
        {
            free(temp);
            if ((*context)->type == CONTEXT_NUM_POSSIBLE)
            {
                if (input[*i] == '<' || input[*i] == '>')
                {
                    (*context)->type = CONTEXT_NUM;
                }
            }
            return 0;
        }
        free(temp);

        if ((*context)->type == CONTEXT_NUM_POSSIBLE)
        {
            if (!is_in("0123456789", input[*i]))
            {
                (*context)->type = CONTEXT_TOKEN;
            }
        }
    }

    return input[*i];
}

static char lex_squote(char *input, size_t *i, struct lexer_context **context)
{
    if (input[(*i)] == '\'')
    {
        (*context)->single_quoted = false;
    }
    return input[*i];
}

static char lex_dquote(char *input, size_t *i, struct lexer_context **context)
{
    if ((*context)->escaped)
    {
        (*context)->escaped = false;
        if (input[*i] == '\n')
        {
            (*context)->token_len -= 1;
            (*i)++;
        }
    }
    else
    {
        if (input[(*i)] == '"')
        {
            (*context)->double_quoted = false;
        }
        else if (input[*i] == '\\' && is_in("$`\"\\\n", input[(*i) + 1]))
        {
            (*context)->escaped = true;
        }
    }
    return input[*i];
}

static char lex_normal(char *input, size_t *i, struct lexer_context **context)
{
    if ((*context)->escaped)
    {
        (*context)->escaped = false;
        if (input[*i] == '\n')
        {
            (*context)->token_len -= 2;
            return (*context)->buffer[(*context)->token_len];
        }
        return input[*i];
    }
    if (isblank(input[*i]))
    {
        (*i)++;
        return 0;
    }
    else if (input[*i] == '\\')
    {
        (*context)->escaped = true;
    }
    else if (input[*i] == '\'')
    {
        (*context)->single_quoted = true;
    }
    else if (input[*i] == '"')
    {
        (*context)->double_quoted = true;
    }
    else if (input[*i] == '#' && (*context)->token_len == 0)
    {
        while (input[*i] != 0 && input[*i] != '\n')
        {
            (*i)++;
        }
        return 0;
    }

    if ((*context)->type == CONTEXT_NEW)
    {
        set_context_type(input[*i], context);
    }
    return handle_char(input, i, context);
}

// Lex the current token until it's end
static struct token *tokenize(char *input, size_t *i)
{
    struct token *token = calloc(1, sizeof(struct token));
    struct lexer_context *context = create_context(input);

    if (!token || !context)
    {
        free(token);
        free(context);
        fprintf(stderr, "tokenize: Cannot calloc\n");
        return NULL;
    }

    char c = 1;
    while ((c != 0 || context->token_len == 0) && input[*i] != 0)
    {
        if (context->single_quoted)
        {
            c = lex_squote(input, i, &context);
        }
        else if (context->double_quoted)
        {
            c = lex_dquote(input, i, &context); // change to dquote
        }
        else
        {
            c = lex_normal(input, i, &context);
        }

        // if c is the null character, it means we are at a delimiter
        if (c != 0)
        {
            (*i)++;
            context->buffer[context->token_len++] = c;
        }
    }

    if (context->token_len == 0)
    {
        free(token);
        free(context->buffer);
        free(context);
        return NULL;
    }
    context->buffer[context->token_len] = 0;
    token->word = build_string(context->buffer, context->token_len);

    if (context->double_quoted || context->single_quoted || context->escaped)
    {
        token->type = LEXER_ERROR;
    }
    else
    {
        token->type = set_token_type(context);
    }

    free(context->buffer);
    free(context);
    return token;
}

void lexer_free(struct token *token)
{
    struct token *temp = token;
    while (token)
    {
        token = token->next;
        free(temp->word);
        free(temp->word_expanded);
        free(temp);
        temp = token;
    }
}

struct token *lexer(char *input)
{
    struct token *token = calloc(1, sizeof(struct token));
    if (!token)
    {
        fprintf(stderr, "lexer: Cannot calloc lexer!\n");
        return NULL;
    }
    size_t i = 0;

    struct token *prev = NULL;
    struct token *temp = token;
    bool in_for = false;
    while (input[i])
    {
        struct token *new = tokenize(input, &i);
        if (new == NULL)
        {
            continue;
        }
        if (new->type == LEXER_ERROR)
        {
            lexer_free(token);
            new->value = VAL_ERROR;
            return new; // Returns a token with token_error
        }
        // changer le type du token en fonction de la string
        grammar_rules(new, prev, &in_for);
        temp->next = new;
        prev = new;
        temp = temp->next;
    }
    token = add_eof_token(token);
    temp = token->next;
    free(token);

    return temp;
}

void lexer_print(struct token *lexer)
{
    if (lexer->type == LEXER_EOF)
    {
        printf("EOF: %d\n", lexer->value);
        return;
    }
    switch (lexer->type)
    {
    case LEXER_WORD:
        printf("Word: %s %d\n", lexer->word, lexer->value);
        break;
    case LEXER_IO_NUMBER:
        printf("Num: %s %d\n", lexer->word, lexer->value);
        break;
    case LEXER_OPERATOR:
        printf("Operator: %s %d\n", lexer->word, lexer->value);
        break;
    case LEXER_NEWLINE:
        printf("Newline %d\n", lexer->value);
        break;
    case LEXER_RESERVED:
        printf("Reserved: %s %d\n", lexer->word, lexer->value);
        break;
    default:
        printf("JE sais pas type : %s\n", lexer->word);
    }

    lexer_print(lexer->next);
}
