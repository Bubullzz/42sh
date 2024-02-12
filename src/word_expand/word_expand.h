#ifndef WORD_EXPAND_H
#define WORD_EXPAND_H

#include <stdbool.h>
#include <stdlib.h>

#include "variables.h"

// Helper for expanding word
struct char_context
{
    size_t word_len;

    bool single_quoted;
    bool double_quoted;
    bool escaped;

    bool error;

    char *buffer;
    size_t buffer_len;

    char **input_split;
    size_t cur_word;
    size_t cur_word_len;
};

char **word_expand(char *word, struct variables *variables);
void free_expanded(char **expanded);

#endif /* ! WORD_EXPAND_H */
