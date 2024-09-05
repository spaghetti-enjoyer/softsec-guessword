#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<unistd.h>

// #include <crypt.h>

int main(void)
{
    char *password = "iloveyou";
    char *salt = "$M9";

    char *res = crypt(salt, password);

    printf("%s\n", res);
    return 0;
}