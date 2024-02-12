#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "exec.h"

static int is_zero(char *str)
{
    int i = 0;
    int n = strlen(str);
    while (i < n && str[i] == 0)
    {
        i++;
    }
    return i == n;
}

static int open_file(char *redir, char *file)
{
    int fd_file;
    if (strcmp(redir, ">>") == 0)
        fd_file = open(file, O_CREAT | O_WRONLY | O_APPEND, 0644);
    else if (strcmp(redir, "<>") == 0)
        fd_file = open(file, O_CREAT | O_RDWR | O_TRUNC, 0644);
    else if (redir[0] == '<')
        fd_file = open(file, O_RDONLY, 0644);
    else
        fd_file = open(file, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    return fd_file;
}

static int redir_input(struct ast *redir, struct ast *exec)
{
    int fd_file = open_file(redir->string, redir->left->string);
    int fd_base = redir->value == -1 ? STDIN_FILENO : redir->value;
    int fd_copy = dup(fd_base);
    dup2(fd_file, fd_base);

    exec_redir(redir->right, exec);

    dup2(fd_copy, fd_base);

    close(fd_file);
    close(fd_copy);
    return 0;
}

static int redir_output(struct ast *redir, struct ast *exec)
{
    int fd_file = open_file(redir->string, redir->left->string);
    int fd_base = redir->value == -1 ? STDOUT_FILENO : redir->value;
    int fd_copy = dup(fd_base);
    dup2(fd_file, fd_base);

    exec_redir(redir->right, exec);

    dup2(fd_copy, fd_base);

    close(fd_file);
    close(fd_copy);
    return 0;
}

static int redir_lessand(struct ast *redir, struct ast *exec)
{
    int fd_changed = atoi(redir->left->string);
    if (fd_changed == 0 && is_zero(redir->left->string))
    {
        fprintf(stderr, "redir_lessand: Undefined behaviour\n");
        return 1;
    }

    if (fcntl(fd_changed, F_GETFD) == -1)
    {
        fprintf(stderr, "redir_greatand: fd not open\n");
        return 1;
    }

    int fd_base = redir->value == -1 ? STDOUT_FILENO : redir->value;
    int fd_copy = dup(fd_base);
    dup2(fd_changed, fd_base);

    exec_redir(redir->right, exec);

    dup2(fd_copy, fd_base);

    close(fd_copy);
    return 0;
}

static int redir_greatand(struct ast *redir, struct ast *exec)
{
    int fd_changed = atoi(redir->left->string);

    if (fd_changed == 0 && is_zero(redir->left->string))
    {
        fprintf(stderr, "redir_greatand: Undefined behaviour\n");
        return 1;
    }

    if (fcntl(fd_changed, F_GETFD) == -1)
    {
        fprintf(stderr, "redir_greatand: fd not open\n");
        return 1;
    }

    int fd_base = redir->value == -1 ? STDIN_FILENO : redir->value;
    int fd_copy = dup(fd_base);
    dup2(fd_changed, fd_base);

    exec_redir(redir->right, exec);

    dup2(fd_copy, fd_base);

    close(fd_copy);
    return 0;
}

static int redir_greatless(struct ast *redir, struct ast *exec)
{
    int fd_file = open_file(redir->string, redir->left->string);

    int fd_base0 = redir->value == -1 ? STDIN_FILENO : redir->value;
    int fd_copy0 = dup(fd_base0);
    dup2(fd_file, fd_base0);

    int fd_base1 = redir->value == -1 ? STDOUT_FILENO : redir->value;
    int fd_copy1 = dup(fd_base1);
    dup2(fd_file, fd_base1);

    exec_redir(redir->right, exec);

    dup2(fd_copy0, fd_base0);
    dup2(fd_copy1, fd_base1);

    close(fd_file);
    close(fd_copy0);
    close(fd_copy1);
    return 0;
}

int redir_handle(struct ast *redir, struct ast *exec)
{
    if (strcmp(redir->string, ">") == 0 || strcmp(redir->string, ">|") == 0
        || strcmp(redir->string, ">>") == 0)
        return redir_output(redir, exec);
    if (strcmp(redir->string, ">&") == 0)
        return redir_lessand(redir, exec);
    else if (strcmp(redir->string, "<") == 0)
        return redir_input(redir, exec);
    else if (strcmp(redir->string, "<&") == 0)
        return redir_greatand(redir, exec);
    else if (strcmp(redir->string, "<>") == 0)
        return redir_greatless(redir, exec);
    else
        return 1;
}
