#include "ast.h"

#include <err.h>
#include <stdio.h>
#include <stdlib.h>

struct ast *ast_new(enum ast_type type)
{
    struct ast *new = calloc(1, sizeof(struct ast));
    if (!new)
        return NULL;
    new->type = type;
    return new;
}

void ast_print(struct ast *ast)
{
    char tab[10][10] = { [COMMAND] = "command",
                         [DO_BOTH] = "do both",
                         [IF] = "if",
                         [IF_CHOICE] = "if_choice" };
    if (ast == 0)
        return;
    printf("{ %s ", tab[ast->type]);
    ast_print(ast->left);
    ast_print(ast->right);
    printf(" }");
}

void ast_free(struct ast *ast)
{
    if (ast == NULL)
        return;

    ast_free(ast->left);
    ast_free(ast->right);
    free(ast);
}
