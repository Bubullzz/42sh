#ifndef EXEC_H
#define EXEC_H

#include "../ast/ast.h"
#include "../word_expand/variables.h"

// Executes the tree and sets variables
int exec_tree(struct ast *tree, struct variables *v);

// Executes the tree
int exec(struct ast *tree);

int exec_redir(struct ast *redir, struct ast *exec);

// Redirection
int redir_handle(struct ast *redir, struct ast *exec);

#endif /* ! EXEC_H */
