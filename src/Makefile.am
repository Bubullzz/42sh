bin_PROGRAMS = 42sh

SUBDIRS = ast \
		  command \
		  exec \
		  io_backend \
		  lexer \
		  parser \
		  word_expand

42sh_SOURCES = main.c

42sh_CFLAGS = -Wall -Wextra -Werror -Wvla -std=c99 -pedantic

42sh_CPPFLAGS = -I$(top_srcdir)/src

42sh_LDADD = \
	exec/libexec.a \
	word_expand/libwordex.a \
	command/libcommand.a \
	io_backend/libio.a \
	lexer/liblexer.a \
	parser/libparser.a \
	ast/libast.a
