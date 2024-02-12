#include <criterion/criterion.h>
#include <string.h>

#include "../word_expand.h"

Test(Basics, Empty)
{
    char *word = "";
    char **word_expanded = word_expand(word, NULL);
    cr_expect_eq(word_expanded[0], NULL);

    free_expanded(word_expanded);
}

Test(Basics, Simple1)
{
    char *word = "echo";
    char **word_expanded = word_expand(word, NULL);
    cr_expect_eq(strcmp(word_expanded[0], "echo"), 0);
    cr_expect_eq(word_expanded[1], NULL);

    free_expanded(word_expanded);
}

Test(Basics, Simple2)
{
    char *word = "'echo'";
    char **word_expanded = word_expand(word, NULL);
    cr_expect_eq(strcmp(word_expanded[0], "echo"), 0);
    cr_expect_eq(word_expanded[1], NULL);

    free_expanded(word_expanded);
}

Test(Basics, Simple3)
{
    char *word = "\"echo\"";
    char **word_expanded = word_expand(word, NULL);
    cr_expect_eq(strcmp(word_expanded[0], "echo"), 0);
    cr_expect_eq(word_expanded[1], NULL);

    free_expanded(word_expanded);
}

Test(Basics, Simple4)
{
    char *word = "'#yes'";
    char **word_expanded = word_expand(word, NULL);
    cr_expect_eq(strcmp(word_expanded[0], "#yes"), 0);
    cr_expect_eq(word_expanded[1], NULL);

    free_expanded(word_expanded);
}

Test(Basics, Quotes1)
{
    char *word = "\" \\\"aled\"";
    char **word_expanded = word_expand(word, NULL);
    cr_expect_eq(strcmp(word_expanded[0], " \"aled"), 0);
    cr_expect_eq(word_expanded[1], NULL);

    free_expanded(word_expanded);
}

Test(Basics, Quotes2)
{
    char *word = "\"test\"test\"test\"";
    char **word_expanded = word_expand(word, NULL);
    cr_expect_eq(strcmp(word_expanded[0], "testtesttest"), 0);
    cr_expect_eq(word_expanded[1], NULL);

    free_expanded(word_expanded);
}

Test(Basics, Quotes3)
{
    char *word = "\"test\"test\"te'test'st\"";
    char **word_expanded = word_expand(word, NULL);
    cr_expect_eq(strcmp(word_expanded[0], "testtestte'test'st"), 0);
    cr_expect_eq(word_expanded[1], NULL);

    free_expanded(word_expanded);
}

Test(Basics, Quotes4)
{
    char *word = "'te\"test\"st'";
    char **word_expanded = word_expand(word, NULL);
    cr_expect_eq(strcmp(word_expanded[0], "te\"test\"st"), 0);
    cr_expect_eq(word_expanded[1], NULL);

    free_expanded(word_expanded);
}

Test(Basics, Quotes5)
{
    char *word = "\"test\\\"test\\\"test\"";
    char **word_expanded = word_expand(word, NULL);
    cr_expect_eq(strcmp(word_expanded[0], "test\"test\"test"), 0);
    cr_expect_eq(word_expanded[1], NULL);

    free_expanded(word_expanded);
}

Test(Basics, Escaped1)
{
    char *word = "2\\>a";
    char **word_expanded = word_expand(word, NULL);
    cr_expect_eq(strcmp(word_expanded[0], "2>a"), 0);
    cr_expect_eq(word_expanded[1], NULL);

    free_expanded(word_expanded);
}

Test(Basics, Escaped2)
{
    char *word = "'abc \\\n def'";
    char **word_expanded = word_expand(word, NULL);
    cr_expect_eq(strcmp(word_expanded[0], "abc \\\n def"), 0);
    cr_expect_eq(word_expanded[1], NULL);

    free_expanded(word_expanded);
}

Test(Basics, Var1)
{
    char *argv[2] = { "./a.out", NULL };
    struct variables *var = init_variables(argv);

    char *word = "$#";
    char **word_expanded = word_expand(word, var);
    cr_expect_eq(strcmp(word_expanded[0], "0"), 0);
    cr_expect_eq(word_expanded[1], NULL);

    free_expanded(word_expanded);
    free_variables(var);
}

Test(Basics, Var2)
{
    char *argv[3] = { "./a.out", "abc", NULL };
    struct variables *var = init_variables(argv);

    char *word = "$@";
    char **word_expanded = word_expand(word, var);
    cr_expect_eq(strcmp(word_expanded[0], "abc"), 0);
    cr_expect_eq(word_expanded[1], NULL);

    free_expanded(word_expanded);
    free_variables(var);
}

Test(Basics, Var3)
{
    char *argv[2] = { "./a.out", NULL };
    struct variables *var = init_variables(argv);

    add_user_var(var, "test=bonjour");

    char *word = "$test";
    char **word_expanded = word_expand(word, var);
    cr_expect_eq(strcmp(word_expanded[0], "bonjour"), 0);
    cr_expect_eq(word_expanded[1], NULL);

    free_expanded(word_expanded);
    free_variables(var);
}

Test(Basics, Var4)
{
    char *argv[2] = { "./a.out", NULL };
    struct variables *var = init_variables(argv);

    add_user_var(var, "test0=ohayou");
    add_user_var(var, "test1=gozaimasu");

    char *word = "$test0 $test1";
    char **word_expanded = word_expand(word, var);
    cr_expect_eq(strcmp(word_expanded[0], "ohayou"), 0);
    cr_expect_eq(strcmp(word_expanded[1], "gozaimasu"), 0);
    cr_expect_eq(word_expanded[2], NULL);

    free_expanded(word_expanded);
    free_variables(var);
}

Test(Basics, Var5)
{
    char *argv[2] = { "./a.out", NULL };
    struct variables *var = init_variables(argv);

    add_user_var(var, "test0=itadakimasu");
    add_user_var(var, "test1=gozaimasu");

    char *word = "$test2$test0";
    char **word_expanded = word_expand(word, var);
    cr_expect_eq(strcmp(word_expanded[0], "itadakimasu"), 0);
    cr_expect_eq(word_expanded[1], NULL);

    free_expanded(word_expanded);
    free_variables(var);
}

Test(Basics, Var6)
{
    char *argv[2] = { "./a.out", NULL };
    struct variables *var = init_variables(argv);

    add_user_var(var, "test0=itadakimasu");
    add_user_var(var, "test1=gozaimasu");

    char *word = "\"${test0} test\"";
    char **word_expanded = word_expand(word, var);
    cr_expect_eq(strcmp(word_expanded[0], "itadakimasu test"), 0);
    cr_expect_eq(word_expanded[1], NULL);

    free_expanded(word_expanded);
    free_variables(var);
}

Test(Basics, Var7)
{
    char *argv[2] = { "./a.out", NULL };
    struct variables *var = init_variables(argv);

    add_user_var(var, "test='abc'");

    char *word = "$test";
    char **word_expanded = word_expand(word, var);
    cr_expect_eq(strcmp(word_expanded[0], "abc"), 0);
    cr_expect_eq(word_expanded[1], NULL);

    free_expanded(word_expanded);
    free_variables(var);
}

Test(Basics, Var8)
{
    char *argv[2] = { "./a.out", NULL };
    struct variables *var = init_variables(argv);

    add_user_var(var, "a=\"cho test\"");

    char *word = "e$a";
    char **word_expanded = word_expand(word, var);
    cr_expect_eq(strcmp(word_expanded[0], "echo"), 0);
    cr_expect_eq(strcmp(word_expanded[1], "test"), 0);
    cr_expect_eq(word_expanded[2], NULL);

    free_expanded(word_expanded);
    free_variables(var);
}
