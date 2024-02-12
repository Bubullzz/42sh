#!/bin/sh

cd ..
autoreconf --install
CPPFLAGS="-I$(top_srcdir)/src -g -fsanitize=address" \
CFLAGS=" -Wall -Wextra -Werror -Wvla -std=c99 -pedantic -O0 --coverage -fPIC -g -fsanitize=address" \
LDFLAGS="-fsanitize=address" \
LDLIBS="-lgcov" \
./configure
make
