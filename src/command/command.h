#ifndef COMMAND_H
#define COMMAND_H

int command_false(void);

int command_true(void);

int command_echo(char **args);

/*
** Used for testing, takes input from stdin and adds 42 before and 69 after
*/
int c69a42t(void);

int builtin_exit(char *code);
int builtin_cd(char *path);
int builtin_export(char **args);
int builtin_dot(char **args);
int builtin_unset(char **args);

#endif /* ! COMMAND_H */
