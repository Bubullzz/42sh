#ifndef GRAMMAR_RULES_H
#define GRAMMAR_RULES_H

#include <stdbool.h>
#include <stdio.h>

#include "lexer.h"

void grammar_rules(struct token *curr, struct token *prev, bool *in_for);

#endif /* ! GRAMMAR_RULES_H */
