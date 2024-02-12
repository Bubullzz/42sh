#ifndef HANDLE_DOLLAR_H
#define HANDLE_DOLLAR_H

#include <stdlib.h>

#include "variables.h"
#include "word_expand.h"

void handle_dollar(char *input, size_t *i, struct char_context **context,
                   struct variables *variables);

#endif /* ! HANDLE_DOLLAR_H */
