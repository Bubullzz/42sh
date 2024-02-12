#ifndef AST_H
#define AST_H

#include <unistd.h>

#include "../command/command.h"

enum ast_type
{
    COMMAND,
    DO_BOTH,
    AND,
    OR,
    PIPE,
    NEG,
    IF,
    IF_CHOICE, // Used in the if execution: execute left child when condition is
               // true, right child otherwise
    FOR,
    BLANK,
    WORD,
    REDIR,
    WHILE,
    UNTIL
};

/**
 * This very simple AST structure should be sufficient for such a simple AST.
 * It is however, NOT GOOD ENOUGH for more complicated projects, such as a
 * shell. Please read the project guide for some insights about other kinds of
 * ASTs.
 */
struct ast
{
    enum ast_type type; ///< The kind of node we're dealing with
    struct ast *left; ///< The left branch if any, unuary or binary
    struct ast *right; ///< The right branch of the binary node
    char *string; // general purpose string
    int value; // general purpose integer
};

/**
 ** \brief Allocate a new ast with the given type
 */
struct ast *ast_new(enum ast_type type);

void ast_print(struct ast *ast);

/**
 ** \brief Recursively free the given ast
 */
void ast_free(struct ast *ast);

#endif /* ! AST_H */
