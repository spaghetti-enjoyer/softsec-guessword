#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


#define MAX_SIZE 100000
int map_size = 0;
char passwords[MAX_SIZE][100];
bool present[MAX_SIZE];


void insert(char *new_password)
{
    strcpy(passwords[map_size], new_password);
    present[map_size] = false;
    map_size++;
}


int find(char *guess_password)
{
    for (int i = 0; i < map_size; i++)
    {
        if (strcmp(guess_password, passwords[i]) == 0)
        {
            return i;
        }
    }
    return -1;
}


void mark_all_occurances(char *known_password)
{
    for (int i = 0; i < map_size; i++)
    {
        if (strcmp(known_password, passwords[i]) == 0)
        {
            present[i] = true;
        }
    }
}


void process_common_input_file(FILE *input)
{
    char common_password[100];
    while (fscanf(input, "%99s", common_password) == 1)
    {
        int pos = find(common_password);
        if (pos != -1)
        {
            mark_all_occurances(common_password);
        }
    }
}


void read_all_common_inputs(char **file_names, int n)
{
    for (int i = 0; i < n; i++)
    {
        FILE *input = fopen(file_names[i], "r");
        if (!input)
        {
            printf("Error opening rainbow file.\n");
            continue;
        }
        process_common_input_file(input);
        fclose(input);
    }
}

int main(void)
{
    // read plaintext passwords

    FILE *input = fopen("training-plain.txt", "r");
    if (!input)
    {
        printf("unable to open plain file.\n");
        return 1;
    }

    char line[100];
    while (fscanf(input, "%99s", line) == 1)
    {
        int start_index = 6;
        int end_index = strlen(line) - 1;
        char *password_buf = malloc((end_index - start_index + 1) * sizeof(char));

        strncpy(password_buf, line + start_index + 1, end_index - start_index);

        insert(password_buf);
        free(password_buf);

    }

    // printf("ok\n");

    // common passwords

    int rainbow_files = 12;
    char *inputs[] = {
                    "top_250_raw.txt", 
                    "unique_words.txt", 
                    "all_caps.txt", 
                    "one_capital.txt", 
                    "names.txt",
                    "common_names.txt",
                    "funny_letters.txt",
                    "zorz.txt",
                    "xor.txt",
                    "birthdays.txt",
                    "two_word_combinations.txt",
                    "up_to_million.txt"};

    read_all_common_inputs(inputs, rainbow_files);

    FILE *output = fopen("yet_undiscovered.txt", "w");
    if (!output)
    {
        printf("Unable to create output file.\n");
        return 1;
    }

    // save results

    int counter_found = 0;
    int counter_not_found = 0;
    for (int i = 0; i < map_size; i++)
    {
        if (present[i] == false)
        {
            fprintf(output, "%s\n", passwords[i]);
            counter_not_found++;
        }
        else
        {
            counter_found++;
        }
    }

    printf("total discovered: %i\ntotal undiscovered: %i\n",counter_found, counter_not_found);

    fclose(input);
    fclose(output);
}