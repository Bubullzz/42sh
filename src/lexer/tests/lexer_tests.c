#include <criterion/criterion.h>
#include <string.h>

#include "../grammar_rules.h"
#include "../lexer.h"

Test(Basics, Empty)
{
    char *input = "";
    struct token *lex = lexer(input);

    cr_expect_eq(lex->type, LEXER_EOF);
    lexer_free(lex);
}

Test(Basics, Simple1)
{
    char *input = "echo A;";
    struct token *lex = lexer(input);

    struct token *temp = lex;
    cr_expect_eq(temp->value, VAL_WORD);
    cr_expect_eq(strcmp(temp->word, "echo"), 0);

    temp = temp->next;
    cr_expect_eq(temp->value, VAL_WORD);
    cr_expect_eq(strcmp(temp->word, "A"), 0);

    temp = temp->next;
    cr_expect_eq(temp->value, OPE_SEMI);
    cr_expect_eq(strcmp(temp->word, ";"), 0);

    temp = temp->next;
    cr_expect_eq(temp->value, VAL_EOF);

    lexer_free(lex);
}

Test(Basics, Simple_if_test)
{
    char *input = "if true echo A; else echo B; fi";
    struct token *lex = lexer(input);

    struct token *temp = lex;
    cr_expect_eq(temp->value, RES_IF);
    cr_expect_eq(strcmp(temp->word, "if"), 0);

    temp = temp->next;
    cr_expect_eq(temp->value, VAL_WORD);
    cr_expect_eq(strcmp(temp->word, "true"), 0);

    temp = temp->next;
    cr_expect_eq(temp->value, VAL_WORD);
    cr_expect_eq(strcmp(temp->word, "echo"), 0);

    temp = temp->next;
    cr_expect_eq(temp->value, VAL_WORD);
    cr_expect_eq(strcmp(temp->word, "A"), 0);

    temp = temp->next;
    cr_expect_eq(temp->value, OPE_SEMI);
    cr_expect_eq(strcmp(temp->word, ";"), 0);

    temp = temp->next;
    cr_expect_eq(temp->value, RES_ELSE);
    cr_expect_eq(strcmp(temp->word, "else"), 0);

    temp = temp->next;
    cr_expect_eq(temp->value, VAL_WORD);
    cr_expect_eq(strcmp(temp->word, "echo"), 0);

    temp = temp->next;
    cr_expect_eq(temp->value, VAL_WORD);
    cr_expect_eq(strcmp(temp->word, "B"), 0);

    temp = temp->next;
    cr_expect_eq(temp->value, OPE_SEMI);
    cr_expect_eq(strcmp(temp->word, ";"), 0);

    temp = temp->next;
    cr_expect_eq(temp->value, RES_FI);
    cr_expect_eq(strcmp(temp->word, "fi"), 0);

    temp = temp->next;
    cr_expect_eq(temp->value, VAL_EOF);

    lexer_free(lex);
}

Test(Basics, Echo_if)
{
    char *input = "echo if";
    struct token *lex = lexer(input);

    struct token *temp = lex;
    cr_expect_eq(temp->value, VAL_WORD);
    cr_expect_eq(strcmp(temp->word, "echo"), 0);

    temp = temp->next;
    cr_expect_eq(temp->value, VAL_WORD);
    cr_expect_eq(strcmp(temp->word, "if"), 0);

    temp = temp->next;
    cr_expect_eq(temp->value, VAL_EOF);

    lexer_free(lex);
}

Test(Basics, Single_quote)
{
    char *input = "echo 'test 4 '";
    struct token *lex = lexer(input);

    struct token *temp = lex;
    cr_expect_eq(temp->value, VAL_WORD);
    cr_expect_eq(strcmp(temp->word, "echo"), 0);

    temp = temp->next;
    cr_expect_eq(temp->value, VAL_WORD);
    cr_expect_eq(strcmp(temp->word, "'test 4 '"), 0);

    temp = temp->next;
    cr_expect_eq(temp->value, VAL_EOF);

    lexer_free(lex);
}

Test(Basics, Comments1)
{
    char *input = "echo test #nope";
    struct token *lex = lexer(input);

    struct token *temp = lex;
    cr_expect_eq(temp->value, VAL_WORD);
    cr_expect_eq(strcmp(temp->word, "echo"), 0);

    temp = temp->next;
    cr_expect_eq(temp->value, VAL_WORD);
    cr_expect_eq(strcmp(temp->word, "test"), 0);

    temp = temp->next;
    cr_expect_eq(temp->value, VAL_EOF);

    lexer_free(lex);
}

Test(Basics, Comments2)
{
    char *input = "echo test#nope";
    struct token *lex = lexer(input);

    struct token *temp = lex;
    cr_expect_eq(temp->value, VAL_WORD);
    cr_expect_eq(strcmp(temp->word, "echo"), 0);

    temp = temp->next;
    cr_expect_eq(temp->value, VAL_WORD);
    cr_expect_eq(strcmp(temp->word, "test#nope"), 0);

    temp = temp->next;
    cr_expect_eq(temp->value, VAL_EOF);

    lexer_free(lex);
}

Test(Basics, Comment_in_single_quote)
{
    char *input = "echo test '#yes'";
    struct token *lex = lexer(input);

    struct token *temp = lex;
    cr_expect_eq(temp->value, VAL_WORD);
    cr_expect_eq(strcmp(temp->word, "echo"), 0);

    temp = temp->next;
    cr_expect_eq(temp->value, VAL_WORD);
    cr_expect_eq(strcmp(temp->word, "test"), 0);

    temp = temp->next;
    cr_expect_eq(temp->value, VAL_WORD);
    cr_expect_eq(strcmp(temp->word, "'#yes'"), 0);

    temp = temp->next;
    cr_expect_eq(temp->value, VAL_EOF);

    lexer_free(lex);
}

Test(Basics, Operators)
{
    char *input = "echo coucou && echo salut #reel ceci est un commentaire";
    struct token *lex = lexer(input);

    struct token *temp = lex;
    cr_expect_eq(temp->value, VAL_WORD);
    cr_expect_eq(strcmp(temp->word, "echo"), 0);

    temp = temp->next;
    cr_expect_eq(temp->value, VAL_WORD);
    cr_expect_eq(strcmp(temp->word, "coucou"), 0);

    temp = temp->next;
    cr_expect_eq(temp->value, OPE_AND_IF);
    cr_expect_eq(strcmp(temp->word, "&&"), 0);

    temp = temp->next;
    cr_expect_eq(temp->value, VAL_WORD);
    cr_expect_eq(strcmp(temp->word, "echo"), 0);

    temp = temp->next;
    cr_expect_eq(temp->value, VAL_WORD);
    cr_expect_eq(strcmp(temp->word, "salut"), 0);

    temp = temp->next;
    cr_expect_eq(temp->value, VAL_EOF);

    lexer_free(lex);
}
Test(Basics, Double_quote)
{
    char *input = "echo \"test\" || echo \" \\\"aled\"";
    struct token *lex = lexer(input);

    struct token *temp = lex;
    cr_expect_eq(temp->value, VAL_WORD);
    cr_expect_eq(strcmp(temp->word, "echo"), 0);

    temp = temp->next;
    cr_expect_eq(temp->value, VAL_WORD);
    cr_expect_eq(strcmp(temp->word, "\"test\""), 0);

    temp = temp->next;
    cr_expect_eq(temp->value, OPE_OR_IF);
    cr_expect_eq(strcmp(temp->word, "||"), 0);

    temp = temp->next;
    cr_expect_eq(temp->value, VAL_WORD);
    cr_expect_eq(strcmp(temp->word, "echo"), 0);

    temp = temp->next;
    cr_expect_eq(temp->value, VAL_WORD);
    cr_expect_eq(strcmp(temp->word, "\" \\\"aled\""), 0);

    temp = temp->next;
    cr_expect_eq(temp->value, VAL_EOF);

    lexer_free(lex);
}

Test(Basics, Operator_escaped)
{
    char *input = "echo test \\&& echo bonjour";
    struct token *lex = lexer(input);

    struct token *temp = lex;
    cr_expect_eq(temp->value, VAL_WORD);
    cr_expect_eq(strcmp(temp->word, "echo"), 0);

    temp = temp->next;
    cr_expect_eq(temp->value, VAL_WORD);
    cr_expect_eq(strcmp(temp->word, "test"), 0);

    temp = temp->next;
    cr_expect_eq(temp->value, VAL_WORD);
    cr_expect_eq(strcmp(temp->word, "\\&"), 0);

    temp = temp->next;
    cr_expect_eq(temp->value, OPE_AMPER);
    cr_expect_eq(strcmp(temp->word, "&"), 0);

    temp = temp->next;
    cr_expect_eq(temp->value, VAL_WORD);
    cr_expect_eq(strcmp(temp->word, "echo"), 0);

    temp = temp->next;
    cr_expect_eq(temp->value, VAL_WORD);
    cr_expect_eq(strcmp(temp->word, "bonjour"), 0);

    temp = temp->next;
    cr_expect_eq(temp->value, VAL_EOF);

    lexer_free(lex);
}

Test(Basics, No_redir)
{
    char *input = "echo 2\\>a";
    struct token *lex = lexer(input);

    struct token *temp = lex;
    cr_expect_eq(temp->value, VAL_WORD);
    cr_expect_eq(strcmp(temp->word, "echo"), 0);

    temp = temp->next;
    cr_expect_eq(temp->value, VAL_WORD);
    cr_expect_eq(strcmp(temp->word, "2\\>a"), 0);

    temp = temp->next;
    cr_expect_eq(temp->value, VAL_EOF);

    lexer_free(lex);
}

Test(Basics, Newline)
{
    char *input = "echo test\\\noui";
    struct token *lex = lexer(input);

    struct token *temp = lex;
    cr_expect_eq(temp->value, VAL_WORD);
    cr_expect_eq(strcmp(temp->word, "echo"), 0);

    temp = temp->next;
    cr_expect_eq(temp->value, VAL_WORD);
    cr_expect_eq(strcmp(temp->word, "testoui"), 0);

    temp = temp->next;
    cr_expect_eq(temp->value, VAL_EOF);

    lexer_free(lex);
}

Test(Basics, Newline_dquote)
{
    char *input = "echo \"test \\\n test\"";
    struct token *lex = lexer(input);

    struct token *temp = lex;
    cr_expect_eq(temp->value, VAL_WORD);
    cr_expect_eq(strcmp(temp->word, "echo"), 0);

    temp = temp->next;
    cr_expect_eq(temp->value, VAL_WORD);
    cr_expect_eq(strcmp(temp->word, "\"test  test\""), 0);

    temp = temp->next;
    cr_expect_eq(temp->value, VAL_EOF);

    lexer_free(lex);
}

Test(Basics, Newline_squote)
{
    char *input = "echo 'abc \\\n def'";
    struct token *lex = lexer(input);

    struct token *temp = lex;
    cr_expect_eq(temp->value, VAL_WORD);
    cr_expect_eq(strcmp(temp->word, "echo"), 0);

    temp = temp->next;
    cr_expect_eq(temp->value, VAL_WORD);
    cr_expect_eq(strcmp(temp->word, "'abc \\\n def'"), 0);

    temp = temp->next;
    cr_expect_eq(temp->value, VAL_EOF);

    lexer_free(lex);
}

Test(Error, DQuotes)
{
    char *input = "echo \"test";
    struct token *lex = lexer(input);

    struct token *temp = lex;
    cr_expect_eq(temp->value, VAL_ERROR);

    lexer_free(lex);
}

Test(Error, SQuotes1)
{
    char *input = "echo 'test";
    struct token *lex = lexer(input);

    struct token *temp = lex;
    cr_expect_eq(temp->value, VAL_ERROR);

    lexer_free(lex);
}

Test(Error, SQuotes2)
{
    char *input = "echo test'";
    struct token *lex = lexer(input);

    struct token *temp = lex;
    cr_expect_eq(temp->value, VAL_ERROR);

    lexer_free(lex);
}

Test(Error, Escaped)
{
    char *input = "echo test\\";
    struct token *lex = lexer(input);

    struct token *temp = lex;
    cr_expect_eq(temp->value, VAL_ERROR);

    lexer_free(lex);
}

Test(Error, Ass_word1)
{
    char *input = "a=b";
    struct token *lex = lexer(input);

    struct token *temp = lex;
    cr_expect_eq(temp->value, VAL_ASSIGNMENT_WORD);

    lexer_free(lex);
}

Test(Error, Ass_word2)
{
    char *input = "a=b=t";
    struct token *lex = lexer(input);

    struct token *temp = lex;
    cr_expect_eq(temp->value, VAL_ASSIGNMENT_WORD);

    lexer_free(lex);
}

Test(Error, Ass_word3)
{
    char *input = "_test12=\"ohayou\"";
    struct token *lex = lexer(input);

    struct token *temp = lex;
    cr_expect_eq(temp->value, VAL_ASSIGNMENT_WORD);

    lexer_free(lex);
}

Test(Error, No_ass_word1)
{
    char *input = "1test12=\"ohayou\"";
    struct token *lex = lexer(input);

    struct token *temp = lex;
    cr_expect_eq(temp->value, VAL_WORD);

    lexer_free(lex);
}

Test(Error, No_ass_word2)
{
    char *input = "te.st12=\"ohayou\"";
    struct token *lex = lexer(input);

    struct token *temp = lex;
    cr_expect_eq(temp->value, VAL_WORD);

    lexer_free(lex);
}
