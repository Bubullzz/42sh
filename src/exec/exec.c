#define _POSIX_C_SOURCE 200112L
#include "exec.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "../command/command.h"
#include "../word_expand/word_expand.h"

struct variables *vars = 0;

// Executes left and right child, used in 6.1.5 Command lists
static int exec_do_both(struct ast *tree);

static int exec_pipe(struct ast *tree);

/*
** Executes tree->right->left if tree->left returned true. Executes
** tree->right->right otherwise
*/
static int exec_if(struct ast *tree);

static int exec_while(struct ast *tree);

static int exec_until(struct ast *tree);

static int exec_for(struct ast *tree);

static int exec_command(struct ast *tree);

static int exec_simple_command(struct ast *exec);

int exec_tree(struct ast *tree, struct variables *v)
{
    vars = v;
    return exec(tree);
}

int exec(struct ast *tree)
{
    if (!tree)
        return 0;
    int code = 0;
    switch (tree->type)
    {
    case DO_BOTH:
        return exec_do_both(tree);
    case AND:
        code = exec(tree->left);
        return code == 0 ? exec(tree->right) : code;
    case OR:
        code = exec(tree->left);
        return code == 0 ? code : exec(tree->right);
    case PIPE:
        return exec_pipe(tree);
    case NEG:
        return !exec(tree->left);
    case IF:
        return exec_if(tree);
    case WHILE:
        return exec_while(tree);
    case UNTIL:
        return exec_until(tree);
    case FOR:
        return exec_for(tree);
    case COMMAND:
        return exec_command(tree);
    default:
        printf("exec: Should not happend\n");
        exit(1);
    }
    return 1;
}

static int exec_do_both(struct ast *tree)
{
    exec(tree->left);
    return exec(tree->right);
}

static int exec_pipe(struct ast *tree)
{
    int pipefds[2];
    pipe(pipefds);
    int stdout_cp = dup(STDOUT_FILENO);
    int stdin_cp = dup(STDIN_FILENO);
    int pid1 = fork();
    int ret = 0;
    if (pid1 == 0)
    {
        close(pipefds[0]);
        if (dup2(pipefds[1], STDOUT_FILENO) == -1)
            printf("exec_pipe: Pas reussi dup2 \n");
        int ret = exec(tree->left);
        fflush(stdout);
        exit(ret == 127 ? EXIT_FAILURE : EXIT_SUCCESS);
    }
    else
    {
        int wstatus1;
        waitpid(pid1, &wstatus1, 0);
        fflush(stdout);
        close(pipefds[1]);
        dup2(stdout_cp, STDOUT_FILENO);
        dup2(pipefds[0], STDIN_FILENO);
        ret = exec(tree->right);
    }
    close(pipefds[0]);
    fflush(stdout);
    dup2(stdin_cp, STDIN_FILENO);
    return ret;
}

static int exec_if(struct ast *tree)
{
    if (exec(tree->left) == 0)
        return exec(tree->right->left);
    else
        return tree->right->right ? exec(tree->right->right) : 0;
}

static int exec_while(struct ast *tree)
{
    // Je mets le 0 car ca rappelle que le 0 == true
    while (exec(tree->left) == 0)
        exec(tree->right);
    return 0;
}

static int exec_until(struct ast *tree)
{
    while (exec(tree->left) != 0)
        exec(tree->right);
    return 0;
}

static int exec_for(struct ast *tree)
{
    char *var_name = tree->left->string;
    struct ast *curr_word = tree->left->left;
    int last = 0;
    for (; curr_word; curr_word = curr_word->right)
    {
        char *buff = malloc(sizeof(char) + strlen(var_name)
                            + strlen(curr_word->string) + 2);
        strcpy(buff, var_name);
        strcat(buff, "=");
        strcat(buff, curr_word->string);
        add_user_var(vars, buff);
        last = exec(tree->right);
        free(buff);
    }
    return last;
}

static int exec_command(struct ast *tree)
{
    struct ast *assign = tree->left->left->right;
    while (assign)
    {
        if (tree->right) // it has a command so assignments are setenv
        {
            char *assignstr = assign->string;
            char *first = strtok(assignstr, "=");
            char *second = strtok(NULL, "=");
            setenv(first, second ? second : "", 1);
        }
        else // no command, so assignments are shell variables
        {
            add_user_var(vars, assign->string);
        }
        assign = assign->right;
    }
    return exec_redir(tree->left->right, tree->right);
}

int exec_redir(struct ast *redir, struct ast *exec)
{
    // Finished parcouring the redirections branch, so it's time for execution
    if (!redir)
    {
        if (exec)
        {
            int code = exec_simple_command(exec);
            fflush(NULL);
            return code;
        }
        return 0;
    }
    else
        return redir_handle(redir, exec);
}

static char **tree_to_args(struct ast *exec)
{
    int index = 0;
    char **ret = calloc(1, sizeof(char *) * (index + 1));
    while (exec)
    {
        char **expanded = word_expand(exec->string, vars);
        for (int i = 0; expanded[i]; i++)
        {
            ret[index] = expanded[i];
            index++;
            ret = realloc(ret, sizeof(char *) * (index + 1));
        }
        free(expanded);
        exec = exec->right;
    }
    ret[index] = NULL;
    return ret;
}

static int command_execution(char **exec_args)
{
    if (!strcmp(exec_args[0], "true"))
        return command_true();
    else if (!strcmp(exec_args[0], "false"))
        return command_false();
    else if (!strcmp(exec_args[0], "echo"))
        return command_echo(&(exec_args[1]));
    else if (!strcmp(exec_args[0], "c69a42t"))
        return c69a42t();
    else if (!strcmp(exec_args[0], "cd"))
        return builtin_cd(exec_args[1]);
    else if (!strcmp(exec_args[0], "export"))
        return builtin_export(&exec_args[1]);
    else if (!strcmp(exec_args[0], "."))
        return builtin_dot(&exec_args[1]);
    else if (!strcmp(exec_args[0], "exit"))
        return builtin_exit(exec_args[1]);
    else if (!strcmp(exec_args[0], "unset"))
        return builtin_unset(&exec_args[1]);
    else
    {
        int pid = fork();
        if (pid == 0)
        {
            execvp(exec_args[0], exec_args);
            exit(127);
        }
        else
        {
            int wstatus;
            waitpid(pid, &wstatus, 0);
            fflush(stdout);
            if (WEXITSTATUS(wstatus) == 127)
            {
                fprintf(stderr, "%s doesn't exist, U stoopid\n", exec_args[0]);
                return 127;
            }
            return WEXITSTATUS(wstatus);
        }
    }
}

static int exec_simple_command(struct ast *exec)
{
    char **args = tree_to_args(exec);
    int ret = args[0] ? command_execution(args) : 0;
    free_expanded(args);
    return ret;
}
