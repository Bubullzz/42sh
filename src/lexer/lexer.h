#ifndef LEXER_H
#define LEXER_H

#include <stdbool.h>
#include <stdio.h>

enum context_type
{
    CONTEXT_NEW,
    CONTEXT_TOKEN,
    CONTEXT_OPE,
    CONTEXT_NUM_POSSIBLE,
    CONTEXT_NUM
};

enum token_type
{
    LEXER_TOKEN,
    LEXER_WORD,
    LEXER_ASSIGNMENT_WORD,
    LEXER_NAME,
    LEXER_NEWLINE,
    LEXER_IO_NUMBER,
    LEXER_OPERATOR,
    LEXER_RESERVED,
    LEXER_EOF, // Put one at the end of the linked list
    LEXER_ERROR
};

enum token_value
{
    OPE_PIPE, // |
    OPE_AMPER, // &
    OPE_SEMI, // ;
    OPE_AND_IF, // &&
    OPE_OR_IF, // ||
    OPE_DSEMI, // ;;
    OPE_INPUT, // <
    OPE_OUTPUT, // >
    OPE_DLESS, // <<
    OPE_DGREAT, // >>
    OPE_LESSAND, // >&
    OPE_GREATAND, // <&
    OPE_LESSGREAT, // <>
    OPE_DLESSDASH, // <<-
    OPE_CLOBBER, // >|
    VAL_RESERVED, // reserved value are below (value = 15)
    RES_IF,
    RES_THEN,
    RES_ELSE,
    RES_ELIF,
    RES_FI,
    RES_DO,
    RES_DONE,
    RES_CASE,
    RES_ESAC,
    RES_WHILE,
    RES_UNTIL,
    RES_FOR,
    RES_LBRACE,
    RES_RBRACE,
    RES_BANG,
    RES_IN,
    VAL_WORD, // value = 32
    VAL_ASSIGNMENT_WORD,
    VAL_NAME,
    VAL_NEWLINE,
    VAL_IO_NUMBER,
    VAL_EOF,
    VAL_ERROR
};

// Helper for creating tokens
struct lexer_context
{
    size_t token_len;

    enum context_type type;
    bool single_quoted;
    bool double_quoted;
    bool escaped;

    char *buffer;
};

struct token
{
    char *word;
    char *word_expanded;
    enum token_type type;
    enum token_value value;
    struct token *next;
};

struct token *lexer(char *input);
void lexer_free(struct token *token);
void lexer_print(struct token *lexer);

#endif /* ! LEXER_H */
