#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>

int main(void)
{

    char *res;

    char *password = "iloveyou";
    char *salt = "$1$M9$";

    res = crypt(password, salt);

    printf("%s\n", res);
    return 0;
}

