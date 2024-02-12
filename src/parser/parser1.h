#ifndef PARSER1_H
#define PARSER1_H

#include "../ast/ast.h"
#include "../lexer/lexer.h"

enum parser_status
{
    PARSER_OK,
    PARSER_UNEXPECTED_TOKEN,
};

int consume_tok(struct token **begin, enum token_value val);

enum parser_status parse_input(struct ast **res, struct token **begin);

enum parser_status parse_list(struct ast **res, struct token **begin);

enum parser_status parse_and_or(struct ast **res, struct token **begin);

enum parser_status parse_pipeline(struct ast **res, struct token **begin);

enum parser_status parse_command(struct ast **res, struct token **begin);

enum parser_status parse_shell_command(struct ast **res, struct token **begin);

#endif /* ! PARSER1_H */
