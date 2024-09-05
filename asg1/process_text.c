#include<stdio.h>
#include<stdlib.h>
#include<string.h>


// int contains()
char *whitespace = "\t\n.,?!:;1234567890-/\'\"@#$%^&*()[]{}_+=~";

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("invalid arg count.\n");
        return 1;
    }

    FILE *book = NULL;
    book = fopen(argv[1], "r");
    if (!book)
    {
        printf("Could not open file.\n");
        return 1;
    }

    FILE *processed_book = NULL;
    processed_book = fopen(argv[2], "w");
    if (!processed_book)
    {
        printf("could not create a new file.\n");
        return 1;
    }

    char ch = 65;
    while ((ch = fgetc(book)) != EOF)
    {
        if (strchr(whitespace, ch) != NULL) continue;

        if (ch == '\n') printf("tf\n");
        fputc(ch, processed_book);
    }

    fclose(book);
    fclose(processed_book);

    return 0;
}