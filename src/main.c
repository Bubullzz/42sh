#define _POSIX_C_SOURCE 200112L
#include <fnmatch.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "ast/ast.h"
#include "command/command.h"
#include "exec/exec.h"
#include "io_backend/io_backend.h"
#include "lexer/lexer.h"
#include "parser/parser1.h"
#include "parser/parser2.h"
#include "word_expand/word_expand.h"

int main(int argc, char **argv)
{
    if (!getenv("PWD"))
    {
        char buf[1000];
        setenv("PWD", getcwd(buf, 1000), 1);
    }
    if (!getenv("HOME"))
    {
        struct passwd *passwd = getpwuid(getuid());
        setenv("HOME", passwd->pw_dir, 1);
    }
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
    free_variables(vars);

    if (code_ret == 2)
    {
        fprintf(stderr, "ALLLOOOO LA POLICE");
    }
    return code_ret;
}
