#include "parser1.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../command/command.h"
#include "../lexer/lexer.h"
#include "parser2.h"

/* Checks if begin contains the right token and consumes it if so */
int consume_tok(struct token **begin, enum token_value val)
{
    if (*begin && (*begin)->value == val)
    {
        *begin = (*begin)->next;
        return 1;
    }
    else
        return 0;
}

/*
** input =
**     list '\n'
**   | list EOF
**   | '\n'
**   | EOF
** ;
*/
enum parser_status parse_input(struct ast **res, struct token **begin)
{
    struct token *begincopy = *begin;
    if (parse_list(res, begin) == PARSER_OK && consume_tok(begin, VAL_NEWLINE))
        return PARSER_OK;
    ast_free(*res);
    *begin = begincopy;
    if (parse_list(res, begin) == PARSER_OK && consume_tok(begin, VAL_EOF))
        return PARSER_OK;
    ast_free(*res);
    *begin = begincopy;
    if (consume_tok(begin, VAL_NEWLINE))
        return PARSER_OK;
    *begin = begincopy;
    if (consume_tok(begin, VAL_EOF))
        return PARSER_OK;
    return PARSER_UNEXPECTED_TOKEN;
}

/*
** list = and_or { ';' and_or } [ ';' ] ;
*/
enum parser_status parse_list(struct ast **res, struct token **begin)
{
    if (parse_and_or(res, begin) == PARSER_OK)
    {
        int unexpected_token_happened = 0;
        struct token *begincopy = *begin;
        while (!unexpected_token_happened && consume_tok(begin, OPE_SEMI))
        {
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
        return PARSER_OK;
    }
    else
        return PARSER_UNEXPECTED_TOKEN;
}

/*
** and_or = pipeline { ( '&&' | '||' ) {'\n'} pipeline } ;
*/
enum parser_status parse_and_or(struct ast **res, struct token **begin)
{
    if (parse_pipeline(res, begin) == PARSER_UNEXPECTED_TOKEN)
        return PARSER_UNEXPECTED_TOKEN;
    struct token *begincopy = *begin;
    int is_and = 0; // used to know if we are parsing && or ||
    while (1)
    {
        if (consume_tok(begin, OPE_AND_IF))
            is_and = 1;
        else if (consume_tok(begin, OPE_OR_IF))
            is_and = 0;
        else
            break;
        while (consume_tok(begin, VAL_NEWLINE))
            ;
        struct ast *rescopy = *res;
        if (parse_pipeline(res, begin) == PARSER_UNEXPECTED_TOKEN)
        {
            *begin = begincopy;
            break;
        }
        else
        {
            enum ast_type op = is_and ? AND : OR;
            struct ast *new = ast_new(op);
            new->left = rescopy;
            new->right = *res;
            *res = new;
            begincopy = *begin;
        }
    }
    return PARSER_OK;
}

/*
** pipeline = [!] command { '|' {'\n'} command } ;
*/
enum parser_status parse_pipeline(struct ast **res, struct token **begin)
{
    int is_neg = consume_tok(begin, RES_BANG);
    if (parse_command(res, begin) == PARSER_OK)
    {
        int unexpected_token_happened = 0;
        struct token *begincopy = *begin;
        while (!unexpected_token_happened && consume_tok(begin, OPE_PIPE))
        {
            while (consume_tok(begin, VAL_NEWLINE))
                ;
            struct ast *rescopy = *res;
            if (parse_command(res, begin) == PARSER_UNEXPECTED_TOKEN)
            {
                unexpected_token_happened = 1;
                *begin = begincopy;
            }
            else
            {
                struct ast *new = ast_new(PIPE);
                new->left = rescopy;
                new->right = *res;
                *res = new;
                begincopy = *begin;
            }
        }
        if (is_neg)
        {
            struct ast *new = ast_new(NEG);
            new->left = *res;
            *res = new;
        }
        return PARSER_OK;
    }
    else
        return PARSER_UNEXPECTED_TOKEN;
}

/*
** command =
**     simple_command
**   | shell_command
** ;
*/
enum parser_status parse_command(struct ast **res, struct token **begin)
{
    struct token *begincopy = *begin;
    if (parse_simple_command(res, begin) == PARSER_OK)
        return PARSER_OK;
    *begin = begincopy;
    if (parse_shell_command(res, begin) == PARSER_OK)
        return PARSER_OK;
    *begin = begincopy;
    return PARSER_UNEXPECTED_TOKEN;
}

/*
** redirection = [IONUMBER] ( '>' | '<' | '>>' | '>&' | '<&' | '>|' | '<>' )
*WORD;
*/
enum parser_status parse_redir(struct ast **res, struct token **begin)
{
    struct token *begincopy = *begin;
    int io_number = -1;
    if ((*begin)->value == VAL_IO_NUMBER)
    {
        io_number = atoi((*begin)->word);
        *begin = (*begin)->next;
    }

    if ((*begin)->value < OPE_INPUT || (*begin)->value > OPE_CLOBBER)
    {
        *begin = begincopy;
        return PARSER_UNEXPECTED_TOKEN;
    }
    struct ast *new = ast_new(REDIR);
    new->string = (*begin)->word;
    new->value = io_number;
    *begin = (*begin)->next;

    if (!begin || (*begin)->value != VAL_WORD)
    {
        *begin = begincopy;
        ast_free(new);
        return PARSER_UNEXPECTED_TOKEN;
    }
    new->left = ast_new(WORD);
    new->left->string = (*begin)->word;
    *begin = (*begin)->next;

    *res = new;
    return PARSER_OK;
}

static int parse_simple_command_prefixes(struct token **begin,
                                         struct ast **redir,
                                         struct ast **assign)
{
    if ((*begin)->value == VAL_ASSIGNMENT_WORD)
    {
        (*assign)->right = ast_new(WORD);
        *assign = (*assign)->right;
        (*assign)->string = (*begin)->word;
        *begin = (*begin)->next;
        return 1;
    }
    else if (parse_redir(&((*redir)->right), begin) == PARSER_OK) // magie noir
    {
        *redir = (*redir)->right;
        return 1;
    }
    else
        return 0;
}

int consume_element_simple_command(struct token **begin, struct ast **redir,
                                   struct ast **word)
{
    if (parse_redir(&((*redir)->right), begin) == PARSER_OK) // magie noir
    {
        *redir = (*redir)->right;
        return 1;
    }
    else if ((*begin)->value == VAL_WORD)
    {
        (*word)->right = ast_new(WORD);
        (*word)->right->string = (*begin)->word;
        *word = (*word)->right;
        *begin = (*begin)->next;
        return 1;
    }
    else
        return 0;
}
/*
** simple_command =
**      prefix { prefix }
**  | { prefix } WORD { element }
    ;

    prefix =
      ASSIGNMENT_WORD
    | redirection
    ;

    element =
      WORD
    | redirection
    ;
*/
enum parser_status parse_simple_command(struct ast **res, struct token **begin)
{
    struct ast *new = ast_new(COMMAND);
    new->left = ast_new(BLANK); // will put all REDIR there
    new->left->left = ast_new(BLANK); // will put all ASSIGN here
    struct ast *curr_redir =
        new->left; // nodes need to be put in the order they are encountered
    struct ast *curr_assign =
        new->left->left; // not needed for assign but allows to treat it the
                         // same as redir

    int found_prefix = 0;
    while (parse_simple_command_prefixes(begin, &curr_redir, &curr_assign))
        found_prefix = 1; // we found at least one prefix;

    if ((*begin)->value == VAL_WORD)
    {
        new->right = ast_new(WORD);
        new->right->string = (*begin)->word;
        struct ast *curr_word = new->right;
        *begin = (*begin)->next;
        while (consume_element_simple_command(begin, &curr_redir, &curr_word))
            ;
        *res = new;
        return PARSER_OK;
    }
    else
    {
        if (found_prefix)
        {
            *res = new;
            return PARSER_OK;
        }
        else
        {
            ast_free(new);
            return PARSER_UNEXPECTED_TOKEN;
        }
    }
}
