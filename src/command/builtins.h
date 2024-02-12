#ifndef BUILTINS_H
#define BUILTINS_H

int builtin_exit(int code);
int builtin_cd(char *path);
int builtin_export(char **args);
int builtin_dot(char **args);

#endif /* ! BUILTINS_H */
