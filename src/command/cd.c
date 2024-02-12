#define _DEFAULT_SOURCE
#define _POSIX_C_SOURCE 200112L
#define _XOPEN_SOURCE 600
#define PATH_MAX 10000

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "command.h"

static int free_error(bool to_free, char *curpath)
{
    if (to_free)
        free(curpath);
    fprintf(stderr, "cd: Error\n");
    return 1;
}

static char *concat_pwd_path(char *pwd, char *curpath)
{
    size_t l_pwd = strlen(pwd);
    size_t l_curpath = strlen(curpath);

    char *ret = calloc(l_pwd + l_curpath + 2, sizeof(char));
    ret = strcat(ret, pwd);
    if (pwd[l_pwd - 1] != '/')
        ret = strcat(ret, "/");
    ret = strcat(ret, curpath);
    return ret;
}

static int cd_oldpwd(void)
{
    char *oldpwd = getenv("OLDPWD");
    char *pwd = getenv("PWD");
    if (!oldpwd)
        oldpwd = pwd;

    if (chdir(oldpwd) != 0)
    {
        fprintf(stderr, "cd: Error chdir\n");
        return 1;
    }

    int r = setenv("OLDPWD", pwd, 1);
    if (r != 0)
    {
        fprintf(stderr, "cd: Error while setting OLDPWD\n");
        return 1;
    }
    r = setenv("PWD", oldpwd, 1);
    if (r != 0)
    {
        fprintf(stderr, "cd: Error while setting PWD\n");
        return 1;
    }
    printf("%s\n", getenv("PWD"));
    return 0;
}

int builtin_cd(char *path)
{
    char *home = getenv("HOME");
    char *curpath = getenv("PWD");
    bool to_free = false;

    if (!path && !home)
    {
        return 2;
    }
    if (!path || !strcmp(path, "~"))
    {
        path = home;
    }
    else if (!strcmp(path, "-"))
    {
        return cd_oldpwd();
    }

    if (path[0] == '/' || path[0] == '.')
    {
        curpath = path;
    }
    else
    {
        curpath = concat_pwd_path(getenv("PWD"), path);
        to_free = true;
    }

    char cannon_path[PATH_MAX + 1];
    char *err = realpath(curpath, cannon_path);

    if (!err)
        return free_error(to_free, curpath);
    if (chdir(cannon_path) != 0)
        return free_error(to_free, curpath);

    int r = setenv("OLDPWD", getenv("PWD"), 1);

    if (r != 0)
        return free_error(to_free, curpath);
    r = setenv("PWD", cannon_path, 1);
    if (r != 0)
        return free_error(to_free, curpath);
    if (to_free)
        free(curpath);

    return 0;
}
