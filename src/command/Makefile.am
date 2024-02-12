lib_LIBRARIES = libcommand.a

libcommand_a_SOURCES =  \
  command.c command.h cd.c exit.c dot.c export.c

liblexer_a_CPPFLAGS = -I$(top_srcdir)/src

liblexer_a_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

noinst_LIBRARIES = libcommand.a
