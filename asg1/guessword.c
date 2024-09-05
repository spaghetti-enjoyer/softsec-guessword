#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<unistd.h>

void test()
{
    char *password = "iloveyou";
    char *salt = "$1$M9";
    char *hashed_password = crypt(password, salt);
    printf("plain: %s\nhashed with crypt: %s\n", password, hashed_password);
}


char *get_salt(char *shadow_line, ssize_t len)
{
    int ctr = 0;
    int start = -1;
    int end = -1;

    for (int i = 0; i < len; i++)
    {
        if (shadow_line[i] == '$') ctr++;

        if (ctr == 1 && start == -1)
        {
            // printf("%i\n", i);j
            start = i;
        }
        if (ctr == 3 && end == -1)
        {
            end = i+1;
            break;
        }
    }

    char *res = malloc((end - start + 1) * sizeof(char));        

    for (int i = 0; i + start < end; i++)
    {
        res[i] = shadow_line[i + start];
    }
    res[end - start] = '\0';

    // printf("%s\n%s\n%i %i %i\n", shadow_line, res, ctr, start, end);
    return res;
}


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

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    // obtaining the salt
    read = getline(&line, &len, shadow);
    // printf("%s\n", line);
    const char *salt = get_salt(line, read);

    while ((read = getline(&line, &len, shadow)) != -1)
    {
        // printf("%s", line);
    }





    printf("\n###   ###   ###   ###   ###\n\n");
    printf("salt: %s\n", salt);


    fclose(passwd);
    fclose(shadow);

    return 0;
}