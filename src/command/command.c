#include "command.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int command_true(void)
{
    return 0;
}

int command_false(void)
{
    return 1;
}

static size_t count_words(char **args)
{
    size_t i = 0;
    while (args[i++])
        ;
    return i - 1;
}

static void print_without_interpretation(char **args, size_t len)
{
    while (len > 0)
    {
        char *temp = args[0];
        char c;

        while ((c = *temp++))
        {
            putchar(c);
        }
        len--;
        args++;
        if (len > 0)
            putchar(' ');
    }
}

static void print_with_interpretation(char **args, size_t len)
{
    while (len > 0)
    {
        char *temp = args[0];
        char c;

        while ((c = *temp++))
        {
            if (c == '\\' && *temp)
            {
                if (*temp == 'n')
                {
                    putchar('\n');
                    temp++;
                    continue;
                }
                else if (*temp == 't')
                {
                    putchar('\t');
                    temp++;
                    continue;
                }
                else if (*temp == '\\')
                {
                    putchar('\\');
                    temp++;
                    continue;
                }
                c = *temp;
            }
            putchar(c);
        }
        len--;
        args++;
        if (len > 0)
            putchar(' ');
    }
}

int command_echo(char **args)
{
    size_t len = count_words(args);
    bool newline = true;
    bool interpretation = false;
    char c;

    while (len > 0 && *args[0] == '-')
    {
        char *temp = args[0] + 1;
        size_t j;
        for (j = 0; temp[j]; j++)
        {
            switch (temp[j])
            {
            case 'e':
            case 'E':
            case 'n':
                break;
            default:
                goto l_echo;
                break;
            }
        }
        if (j == 0)
            goto l_echo;
        while (*temp)
        {
            c = *temp++;
            if (c == 'e' || c == 'E')
                interpretation = c == 'e';
            else if (c == 'n')
                newline = false;
            else
                printf("Should not happen\n");
        }
        args++;
        len--;
    }

l_echo:
    if (!interpretation)
        print_without_interpretation(args, len);
    else
        print_with_interpretation(args, len);
    if (newline)
        putchar('\n');
    return 0;
}

int c69a42t(void)
{
    write(STDOUT_FILENO, "69\n", 3);
    char ch;
    while (read(STDIN_FILENO, &ch, 1))
        write(STDOUT_FILENO, &ch, 1);
    write(STDOUT_FILENO, "42\n", 3);
    return 0;
}
