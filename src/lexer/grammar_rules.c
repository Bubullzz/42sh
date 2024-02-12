#include "grammar_rules.h"

#include <fnmatch.h>
#include <stdio.h>
#include <string.h>

#include "lexer.h"

static void handle_numbers(struct token *token)
{
    token->value = VAL_IO_NUMBER;
    return;
}

static void handle_operators(struct token *token)
{
    if (!strcmp(token->word, "\n"))
    {
        token->type = LEXER_NEWLINE;
        token->value = VAL_NEWLINE;
        return;
    }

    size_t j = 0;
    char *operators[] = { "|",  "&",  ";",  "&&", "||", ";;",  "<",  ">",
                          "<<", ">>", ">&", "<&", "<>", "<<-", ">|", NULL };
    while (operators[j] && strcmp(operators[j], token->word))
    {
        j++;
    }
    if (operators[j] == NULL)
    {
        fprintf(stderr, "There was a problem while lexing an operator\n");
    }
    else
    {
        token->value = j;
    }
    return;
}

static void handle_words(struct token *token, struct token *prev, bool *in_for)
{
    // It means the word CAN be a reserved_word
    if (prev == NULL || prev->type == LEXER_NEWLINE
        || prev->type == LEXER_RESERVED || prev->type == LEXER_OPERATOR)
    {
        size_t j = 0;
        char *reserved_words[] = { "if",    "then", "else", "elif", "fi",
                                   "do",    "done", "case", "esac", "while",
                                   "until", "for",  "{",    "}",    "!",
                                   "in",    NULL };
        while (reserved_words[j] && strcmp(reserved_words[j], token->word))
        {
            j++;
        }
        if (reserved_words[j])
        {
            if (strcmp("for", token->word) == 0)
            {
                *in_for = true;
            }
            token->type = LEXER_RESERVED;
            token->value = VAL_RESERVED + j + 1;
            return;
        }
    }
    if (strcmp("in", token->word) == 0 && *in_for)
    {
        token->type = LEXER_RESERVED;
        token->value = RES_IN;
        *in_for = false;
        return;
    }
    if (strcmp("done", token->word) == 0)
    {
        token->type = LEXER_RESERVED;
        token->value = RES_DONE;
        return;
    }
    // Check for assignment name
    if (fnmatch("?*=?*", token->word, 0) == 0
        && fnmatch("[!a-zA-Z_]*", token->word, 0) != 0
        && fnmatch("*[!a-zA-Z0-9_=]*=*", token->word, 0) != 0)
    {
        if (!prev || strcmp(prev->word, "export"))
        {
            token->value = VAL_ASSIGNMENT_WORD;
            return;
        }
    }

    token->value = VAL_WORD;
}

void grammar_rules(struct token *curr, struct token *prev, bool *in_for)
{
    switch (curr->type)
    {
    case LEXER_WORD:
        handle_words(curr, prev, in_for);
        break;
    case LEXER_OPERATOR:
        handle_operators(curr);
        break;
    case LEXER_IO_NUMBER:
        handle_numbers(curr);
        break;
    default:
        break;
    }
}
