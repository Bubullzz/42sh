lib_LIBRARIES = libparser.a

libparser_a_SOURCES =  \
  parser1.c parser1.h \
  parser2.c parser2.h

liblexer_a_CPPFLAGS = -I$(top_srcdir)/src

liblexer_a_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

noinst_LIBRARIES = libparser.a
