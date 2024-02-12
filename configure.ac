AC_INIT([42sh], [1,0], [aglae.tournois@epita.fr])
AM_INIT_AUTOMAKE([subdir-objects] [foreign])

# tg autotools
AM_SILENT_RULES([yes])

# enable ar for Makefile
AM_PROG_AR

# check if a ranlib is available
# ajoute des indexs pour link plus rapidement
AC_PROG_RANLIB

# check if a compiler is available
AC_PROG_CC

# check if flags exists
AX_COMPILER_FLAGS([], [], [], [-Wall -Werror -Wextra -Wvla -std=c99 -pedantic])

# list of the Makefile to generate with autoconf
# A RAJOUTER
AC_CONFIG_FILES([
                Makefile
                src/Makefile
                src/ast/Makefile
                src/command/Makefile
                src/exec/Makefile
                src/io_backend/Makefile
                src/lexer/Makefile
                src/parser/Makefile
                src/word_expand/Makefile
                tests/Makefile
               ])

# generate the Makefile
AC_OUTPUT
