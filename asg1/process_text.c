#include<stdio.h>
#include<stdlib.h>
#include<string.h>


// int contains()
char *alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \n-_\"\'";
char *whitespace = "\n -_\"\'";

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("invalid arg count.\n");
        return 1;
    }

    for (int i = 1; i < argc; i++)
    {
        FILE *book = NULL;
        book = fopen(argv[i], "r");
        if (!book)
        {
            printf("Could not open file.\n");
            return 1;
        }

        int title_length = strlen(argv[i]);
        // int start_index = 20;

        char output_prefix[] = "preprocessed/";
        char *output_name = malloc(title_length + strlen(output_prefix));
        snprintf(output_name, title_length + strlen(output_prefix) + 1, "%s%s", output_prefix, argv[i]);

        printf("%s\n", output_name);

        FILE *processed_book = NULL;
        processed_book = fopen(output_name, "w");
        if (!processed_book)
        {
            printf("could not create a new file.\n");
            return 1;
        }

        char ch = 65;
        char prev = 65;
        while ((ch = fgetc(book)) != EOF)
        {
            if (strchr(alphabet, ch) == NULL)
            {
                continue;
            }
            if ((strchr(whitespace, ch) != NULL)&& prev == ' ')
            {
                continue;
            }
            if (strchr(whitespace, ch) != NULL) 
            {
                ch = ' ';
            }
            fputc(ch, processed_book);
            prev = ch;
        }

        fclose(book);
        fclose(processed_book);
    }

    return 0;
}