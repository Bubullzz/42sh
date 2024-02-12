#ifndef PARSER2_H
#define PARSER2_H

#include "../ast/ast.h"
#include "../lexer/lexer.h"
#include "parser1.h"

enum parser_status parse_simple_command(struct ast **res, struct token **begin);

enum parser_status parse_rule_if(struct ast **res, struct token **begin);

enum parser_status parse_rule_while(struct ast **res, struct token **begin);

enum parser_status parse_rule_until(struct ast **res, struct token **begin);

enum parser_status parse_rule_for(struct ast **res, struct token **begin);

enum parser_status parse_else_clause(struct ast **res, struct token **begin);

enum parser_status parse_compound_list(struct ast **res, struct token **begin);

#endif /* ! PARSER2_H */
