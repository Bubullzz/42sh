#define BUFFER_SIZE 32
#define _POSIX_C_SOURCE 200809L

#include "io_backend.h"

#include <err.h>
#include <fnmatch.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../lexer/lexer.h"

char *read_stream(FILE *stream)
{
    char *fstr = calloc(BUFFER_SIZE + 1, sizeof(char));
    size_t fstr_maxlen = BUFFER_SIZE + 1;
    size_t fstr_len = 0;

    char *buffer = calloc(BUFFER_SIZE, sizeof(char));
    size_t char_read = fread(buffer, sizeof(char), BUFFER_SIZE, stream);

    while (char_read > 0)
    {
        if (fstr_len + char_read >= fstr_maxlen)
        {
            fstr_maxlen *= 2;
            fstr = realloc(fstr, fstr_maxlen * sizeof(char));
        }
        fstr_len += char_read;
        strncat(fstr, buffer, char_read);
        char_read = fread(buffer, sizeof(char), BUFFER_SIZE, stream);
    }

    free(buffer);
    return fstr;
}

char *io_main(int argc, char **argv)
{
    if (argc > 3)
        errx(2, "Bad usage: ./42sh [(-c STRING | FILE.sh)]");

    FILE *stream = NULL;

    // STDIN
    if (argc == 1)
    {
        if (!isatty(0))
            stream = stdin;
        else
            errx(1, "Bad usage: stdin empty\n");
    }
    // FILE.SH (.sh obligatoire ?)
    else if (argc == 2)
        stream = fopen(argv[1], "r");
    // -c STRING
    else if (argc == 3 && strcmp(argv[1], "-c") == 0)
        stream = fmemopen(argv[2], strlen(argv[2]), "r");
    else
        errx(1, "Bad usage: ./42sh [(-c STRING | FILE.sh)]\n");

    if (!stream)
        errx(1, "Open: Failed to open a stream\n");

    char *fstr = read_stream(stream);

    if (argc != 1)
        fclose(stream);

    return fstr;
}
