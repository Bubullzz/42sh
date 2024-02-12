#ifndef IO_BACKEND_H
#define IO_BACKEND_H

#include <stdio.h>

char *read_stream(FILE *stream);

char *io_main(int argc, char **argv);

#endif /*! IO_BACKEND_H*/
