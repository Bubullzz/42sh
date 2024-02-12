lib_LIBRARIES = libwordex.a

libwordex_a_SOURCES =  \
  word_expand.c word_expand.h \
  variables.c variables.h \
  handle_dollar.c handle_dollar.h

libwordex_a_CPPFLAGS = -I$(top_srcdir)/src

libwordex_a_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

noinst_LIBRARIES = libwordex.a
