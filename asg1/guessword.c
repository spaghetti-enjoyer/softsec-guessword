#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "crypt.h"


int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Not enough arguments\n");
        return 1;
    }

    FILE *passwd = fopen(argv[1], "r");
    FILE *shadow = fopen(argv[2], "r");

    if (!passwd || !shadow)
    {
        printf("Could not open input file.\n");
        return 1;
    }

    // char *line = NULL;
    // size_t len = 0;
    // ssize_t read;

    // while ((read = getline(&line, &len, passwd)) != -1)
    // {
    //     printf("%s", line);
    // }

    // printf("\n###   ###   ###   ###   ###\n\n");

    const char *password = "iloveyou";
    const char *salt = "$1$M9"; // MD5 algorithm identifier and salt

    char *unhashed_pwsd = NULL;
    strcpy(unhashed_pwsd, password);
    strncat(unhashed_pwsd, salt, 5);

    char *hashed_password;

    // Hash the password using the salt
    hashed_password = md5(unhashed_pwsd);

    if (hashed_password == NULL)
    {
        printf("Error hashing the password.\n");
        return 1;
    }

    printf("plain: %s\nhashed with md5: %s\n", password, hashed_password);

    // Clean up
    fclose(passwd);
    fclose(shadow);
    // free(line);

    return 0;
}