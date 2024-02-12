#define PATH_MAX 10000
#define _DEFAULT_SOURCE
#define _POSIX_C_SOURCE 200112L
#define _XOPEN_SOURCE 600
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../ast/ast.h"
#include "../exec/exec.h"
#include "../io_backend/io_backend.h"
#include "../lexer/lexer.h"
#include "../parser/parser1.h"
#include "../parser/parser2.h"
#include "command.h"

static int dot_main(int argc, char **argv)
{
    char *fstr = io_main(argc, argv);
    struct token *lexed = lexer(fstr);
    struct token *lexedcopy = lexed;
    struct ast *res = 0;
    int code_ret = 2;
    if (lexed && lexed->type == LEXER_EOF)
        code_ret = 0;
    struct variables *vars = init_variables(argv);
    while (lexed && lexed->type != LEXER_EOF
           && parse_input(&res, &lexed) == PARSER_OK)
    {
        if (res)
            code_ret = exec_tree(res, vars);
        consume_tok(&lexed, VAL_NEWLINE);
        ast_free(res);
        res = 0;
    }
    lexer_free(lexedcopy);
    free(fstr);
    free(argv);

    if (code_ret == 2)
    {
        fprintf(stderr, "ALLLOOOO LA POLICE");
    }
    return code_ret;
}

static char *get_full_path(char *pwd, char *path)
{
    size_t l_pwd = strlen(pwd);
    size_t l_path = strlen(path);

    char *ret = calloc(l_pwd + l_path + 2, sizeof(char));
    ret = strcat(ret, pwd);
    if (pwd[l_pwd - 1] != '/')
        ret = strcat(ret, "/");
    ret = strcat(ret, path);
    return ret;
}

static int contain_slash(char *str)
{
    size_t i = 0;
    while (str[i] && str[i] != '/')
        i++;
    return str[i];
}

int builtin_dot(char **args)
{
    if (!args[0])
    {
        fprintf(stderr, "dot: Need a script to execute\n");
        return 1;
    }
    char *path = (contain_slash(args[0]) ? getenv("PWD") : getenv("PATH"));
    char *full_path = get_full_path(path, args[0]);
    char cannon_path[PATH_MAX + 1];
    char *err;

    err = realpath(full_path, cannon_path);
    if (!err)
    {
        fprintf(stderr, "dot: Cannot resolver path\n");
        free(full_path);
        return 1;
    }
    free(full_path);

    char **t_args = malloc(sizeof *args * 3);
    t_args[0] = "./42sh";
    t_args[1] = cannon_path;
    t_args[2] = NULL;
    return dot_main(2, t_args);
}
