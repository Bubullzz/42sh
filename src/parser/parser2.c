#include "parser2.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../command/command.h"
#include "../lexer/lexer.h"
#include "parser1.h"

/*
** shell_command = rule_if ;
*/
enum parser_status parse_shell_command(struct ast **res, struct token **begin)
{
    struct token *begincopy = *begin;
    if (parse_rule_if(res, begin) == PARSER_OK)
        return PARSER_OK;

    *begin = begincopy;
    if (parse_rule_while(res, begin) == PARSER_OK)
        return PARSER_OK;

    *begin = begincopy;
    if (parse_rule_until(res, begin) == PARSER_OK)
        return PARSER_OK;

    *begin = begincopy;
    if (parse_rule_for(res, begin) == PARSER_OK)
        return PARSER_OK;
    return PARSER_UNEXPECTED_TOKEN;
}

/*
** rule_if = 'if' compound_list 'then' compound_list [else_clause] 'fi' ;
*/
enum parser_status parse_rule_if(struct ast **res, struct token **begin)
{
    if (!consume_tok(begin, RES_IF))
        return PARSER_UNEXPECTED_TOKEN;

    if (parse_compound_list(res, begin) == PARSER_UNEXPECTED_TOKEN)
        return PARSER_UNEXPECTED_TOKEN;
    struct ast *new = ast_new(IF);
    new->left = *res;
    new->right = ast_new(IF_CHOICE);

    if (!consume_tok(begin, RES_THEN))
    {
        ast_free(new);
        *res = 0;
        return PARSER_UNEXPECTED_TOKEN;
    }

    if (parse_compound_list(res, begin) == PARSER_UNEXPECTED_TOKEN)
    {
        ast_free(new);
        *res = 0;
        return PARSER_UNEXPECTED_TOKEN;
    }
    new->right->left = *res;

    struct token *begincopy = *begin;
    if (parse_else_clause(res, begin) == PARSER_OK)
        new->right->right = *res;
    else
    {
        *begin = begincopy;
        new->right->right = 0;
    }
    if (!consume_tok(begin, RES_FI))
    {
        ast_free(new);
        *res = 0;
        return PARSER_UNEXPECTED_TOKEN;
    }
    *res = new;
    return PARSER_OK;
}

/*
** rule_while = 'while' compound_list 'do' compound_list 'done' ;
*/
enum parser_status parse_rule_while(struct ast **res, struct token **begin)
{
    if (!consume_tok(begin, RES_WHILE))
        return PARSER_UNEXPECTED_TOKEN;

    if (parse_compound_list(res, begin) == PARSER_UNEXPECTED_TOKEN)
        return PARSER_UNEXPECTED_TOKEN;
    struct ast *new = ast_new(WHILE);
    new->left = *res;

    if (!consume_tok(begin, RES_DO))
    {
        ast_free(new);
        *res = 0;
        return PARSER_UNEXPECTED_TOKEN;
    }
    if (parse_compound_list(res, begin) == PARSER_UNEXPECTED_TOKEN)
    {
        ast_free(new);
        *res = 0;
        return PARSER_UNEXPECTED_TOKEN;
    }
    new->right = *res;
    if (!consume_tok(begin, RES_DONE))
    {
        ast_free(new);
        *res = 0;
        return PARSER_UNEXPECTED_TOKEN;
    }
    *res = new;
    return PARSER_OK;
}

/*
** rule_until = 'until' compound_list 'do' compound_list 'done' ;
*/
enum parser_status parse_rule_until(struct ast **res, struct token **begin)
{
    if (!consume_tok(begin, RES_UNTIL))
        return PARSER_UNEXPECTED_TOKEN;

    if (parse_compound_list(res, begin) == PARSER_UNEXPECTED_TOKEN)
        return PARSER_UNEXPECTED_TOKEN;
    struct ast *new = ast_new(UNTIL);
    new->left = *res;

    if (!consume_tok(begin, RES_DO))
    {
        ast_free(new);
        *res = 0;
        return PARSER_UNEXPECTED_TOKEN;
    }
    if (parse_compound_list(res, begin) == PARSER_UNEXPECTED_TOKEN)
    {
        ast_free(new);
        *res = 0;
        return PARSER_UNEXPECTED_TOKEN;
    }
    new->right = *res;
    if (!consume_tok(begin, RES_DONE))
    {
        ast_free(new);
        *res = 0;
        return PARSER_UNEXPECTED_TOKEN;
    }
    *res = new;
    return PARSER_OK;
}

/*
** else_clause =
**     'else' compound_list
**   | 'elif' compound_list 'then' compound_list [else_clause]
** ;
*/
enum parser_status parse_else_clause(struct ast **res, struct token **begin)
{
    struct token *begincopy = *begin;
    // parsing: 'else' compound_list
    if (consume_tok(begin, RES_ELSE))
    {
        if (parse_compound_list(res, begin) == PARSER_OK)
        {
            return PARSER_OK;
        }
    }

    // parsing: 'elif' compound_list 'then' compound_list [else_clause]
    *begin = begincopy;
    if (consume_tok(begin, RES_ELIF))
    {
        if (parse_compound_list(res, begin) == PARSER_OK)
        {
            struct ast *new = ast_new(IF);
            new->left = *res;
            new->right = ast_new(IF_CHOICE);
            if (consume_tok(begin, RES_THEN))
            {
                if (parse_compound_list(res, begin) == PARSER_OK)
                {
                    new->right->left = *res;

                    struct token *begincopy = *begin;
                    if (parse_else_clause(res, begin) == PARSER_OK)
                        new->right->right = *res;
                    else
                    {
                        *begin = begincopy;
                        new->right->right = 0;
                    }
                    *res = new;
                    return PARSER_OK;
                }
            }
            ast_free(new);
        }
    }
    return PARSER_UNEXPECTED_TOKEN;
}

static struct ast *consume_words(struct token **begin)
{
    if ((*begin)->value == VAL_WORD)
    {
        struct ast *res = ast_new(WORD);
        struct ast *curr = res;
        curr->string = (*begin)->word;
        *begin = (*begin)->next;
        for (; (*begin)->value == VAL_WORD; *begin = (*begin)->next)
        {
            curr->right = ast_new(WORD);
            curr = curr->right;
            curr->string = (*begin)->word;
        }
        return res;
    }
    return NULL;
}

/*
** rule_for =
** 'for' WORD ( [';'] | [ {'\n'} 'in' { WORD } ( ';' | '\n' ) ] ) {'\n'} 'do'
*compound_list
** 'done' ;
*/
enum parser_status parse_rule_for(struct ast **res, struct token **begin)
{
    struct token *begincopy = *begin;
    struct ast *four = 0;
    if (consume_tok(begin, RES_FOR))
    {
        if ((*begin)->value != VAL_WORD)
            goto end;
        four = ast_new(FOR);
        four->left = ast_new(WORD);
        four->left->string = (*begin)->word;
        *begin = (*begin)->next;

        consume_tok(begin, OPE_SEMI);
        while (consume_tok(begin, VAL_NEWLINE))
            ;
        if (consume_tok(begin, RES_IN))
        {
            struct ast *values = consume_words(begin);
            four->left->left = values;
            if (!consume_tok(begin, OPE_SEMI)
                && !consume_tok(begin, VAL_NEWLINE))
                goto end;
        }
        while (consume_tok(begin, VAL_NEWLINE))
            ;
        if (!consume_tok(begin, RES_DO))
            goto end;
        if (parse_compound_list(res, begin) == PARSER_OK)
        {
            four->right = *res;
            if (consume_tok(begin, RES_DONE))
            {
                *res = four;
                return PARSER_OK;
            }
            *res = 0;
        }
    }
end:
    ast_free(four);
    *begin = begincopy;
    return PARSER_UNEXPECTED_TOKEN;
}

/*
** compound_list = {'\n'} and_or { ( ';' | '\n' ) {'\n'} and_or } [';'] {'\n'} ;
*/
enum parser_status parse_compound_list(struct ast **res, struct token **begin)
{
    while (consume_tok(begin, VAL_NEWLINE))
        ;
    if (parse_and_or(res, begin) == PARSER_UNEXPECTED_TOKEN)
        return PARSER_UNEXPECTED_TOKEN;
    int unexpected_token_happened = 0;
    struct token *begincopy = *begin;
    while (!unexpected_token_happened
           && (consume_tok(begin, OPE_SEMI) || consume_tok(begin, VAL_NEWLINE)))
    {
        while (consume_tok(begin, VAL_NEWLINE))
            ;
        struct ast *rescopy = *res;
        if (parse_and_or(res, begin) == PARSER_UNEXPECTED_TOKEN)
        {
            unexpected_token_happened = 1;
            *begin = begincopy;
        }
        else
        {
            struct ast *new = ast_new(DO_BOTH);
            new->left = rescopy;
            new->right = *res;
            *res = new;
            begincopy = *begin;
        }
    }
    consume_tok(begin, OPE_SEMI); // poping optional OPE_SEMI
    while (consume_tok(begin, VAL_NEWLINE))
        ;
    return PARSER_OK;
}
