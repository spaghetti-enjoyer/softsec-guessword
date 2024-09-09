#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <unistd.h>


// MAP IMPLEMENTATION

#define MAX_SIZE 10000000
int map_size = 0;
char passwords[MAX_SIZE][100]; 
char hashes[MAX_SIZE][100];


int look_up(char *hash)
{
    for (int i = 0; i < map_size; i++)
    {
        if (strcmp(hash, hashes[i]) == 0)
        {
            return i;
        }
    }
    return -1;
}

void insert_hash(char *new_password, char *new_hash)
{
    strcpy(passwords[map_size], new_password);
    strcpy(hashes[map_size], new_hash);
    map_size++;
}




// void test()
// {
//     char *password = "iloveyou";
//     char *salt = "$1$M9";
//     char *hashed_password = crypt(password, salt);
//     printf("plain: %s\nhashed with crypt: %s\n", password, hashed_password);
// }


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
            end = i;
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


char *unhash_password(char *encrypted_password)
{
    if (0) // timed out
    {
        return NULL;
    }

    int found = look_up(encrypted_password);

    if (found == -1)
    {
        return NULL;
    }

    char *result = malloc((strlen(passwords[found]) + 1) * sizeof(char));
    if (result == NULL)
    {
        printf("failed to allocate memory\n");
        return NULL;
    }

    strcpy(result, passwords[found]);
    return result;
}


int process_line(char *line, int len, char **username, char **password)
{

    int colon_counter = 0;
    int username_end = -1;
    int password_start = -1;
    int password_end = -1;

    for (int i = 0; i < len; i++)
    {
        if (line[i] == ':') colon_counter++;

        if (colon_counter == 1 && username_end == -1)
        {
            username_end = i;
            password_start = i + 1;
        }

        if (colon_counter == 2 && password_end == -1)
        {
            password_end = i;
            break;
        }
    }

    if (username_end == -1 || password_start == -1 || password_end == -1) {
        return 1; // Error: Invalid line format
    }

    *username = malloc((username_end + 1) * sizeof(char));
    *password = malloc((password_end - password_start + 1) * sizeof(char));

    if (*username == NULL || *password == NULL) {
        return 1; // Error: Memory allocation failed
    }

    strncpy(*username, line, username_end);
    (*username)[username_end] = '\0';

    strncpy(*password, line + password_start, password_end - password_start);
    (*password)[password_end - password_start] = '\0';

    return 0;
}


void process_common_input_file(FILE *input, const char *salt)
{
    char common_password[100];
    while (fscanf(input, "%99s", common_password) == 1)
    {
        // printf("%s\n", common_password);
        char *hash = crypt(common_password, salt);
        insert_hash(common_password, hash);

    }
}


void read_all_common_inputs(char **file_names, int n, const char *salt)
{
    for (int i = 0; i < n; i++)
    {
        FILE *input = fopen(file_names[i], "r");
        if (!input)
        {
            printf("Error opening rainbow file %s.\n", file_names[i]);
            continue;
        }
        process_common_input_file(input, salt);
        fclose(input);
    }
}


char **get_all_names_from_line(char *line, int len)
{
    int start_index = -1;
    int end_index = -1;
    int colon_counter = 0;
    // int comma_counter = 0;

    for (int i = 0; i < len; i++)
    {
        if (line[i] == ':')
        {
            colon_counter++;
        }
        if (colon_counter == 4)
        {
            start_index = i+1;
        }
        else if (line[i] == ',')
        {
            // comma_counter++;
            end_index = i;
        }
    }

    if (start_index == -1 || end_index == -1 || start_index >= end_index)
    {
        return NULL; // Invalid indices
    }

    int names_string_size = end_index - start_index + 1;
    char *names_string = malloc(names_string_size * sizeof(char));
    snprintf(names_string, names_string_size, "%.*s", names_string_size - 1, line + start_index);

    if (names_string == NULL)
    {
        return NULL;
    }
    printf("%s\n", names_string);

    int space_counter = 0;
    for (int i = 0; i < names_string_size; i++)
    {
        if (names_string[i] == ' ')
        {
            space_counter++;
        }
    }

    int names_count = space_counter + 1;
    char **all_names = malloc(names_count * sizeof(char *));
    if (all_names == NULL)
    {
        return NULL;
    }

    int prev_end = -1;
    for (int i = 0; i < names_count; i++)
    {
        int current_end = -1;
        for (int j = prev_end + 1; j < names_string_size; j++)
        {
            if (names_string[j] == ' ')
            {
                current_end = j;
            }
        }
        if (current_end == -1)
        {
            return NULL;
        }

        int current_name_size = current_end - prev_end + 1;
        all_names[i] = malloc(current_name_size * sizeof(char));
        if (all_names[i] == NULL)
        {
            return NULL;
        }

        snprintf(all_names[i], current_name_size, "%.*s", current_name_size - 1, names_string + prev_end + 1);
    }

    // for (int i = 0; i < names_count; i++)
    // {
    //     printf("name %i: %s\n", i, all_names[i]);
    // }

    free(names_string);
    return all_names;
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
        printf("Could not open file.\n");
        return 1;
    }


    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    // read /etc/shadow
    char *salt = NULL;
    while ((read = getline(&line, &len, shadow)) != -1)
    {
        // obtaining the salt
        if (salt == NULL)
        {
            salt = get_salt(shadow, read);
        }

        char *passwd_line = NULL;
        size_t passwd_len = 0;
        ssize_t passwd_read;

        if ((read = getline(&passwd_line, &passwd_len, passwd)) == -1)
        {
            // did not read line from etc/passwd for some reason
            continue;
        }

        char **real_names = get_all_names_from_line(passwd_line, passwd_read);
    }

    // // obtaining the salt
    // read = getline(&line, &len, shadow);
    // const char *salt = get_salt(line, read);


    // check passwords involving one's real name




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

    read_all_common_inputs(inputs, rainbow_files, salt);


    int success_counter = 0;
    int fail_counter = 0;
    while ((read = getline(&line, &len, shadow)) != -1)
    {
        // printf("%s", line);

        char *username = NULL;
        char *password = NULL;

        if (process_line(line, read, &username, &password) != 0)
        {
            fail_counter++;
            continue;
        }

        char *unhashed_password = unhash_password(password);

        if (unhashed_password == NULL)
        {
            fail_counter++;
            continue;
        }

        printf("%s:%s\n", username, unhashed_password);
        fflush(stdout);
        success_counter++;

        free(username);
        free(password);
        free(unhashed_password);
    }

    // for (int i = 0; i < map_size; i++)
    // {
    //     printf("%s\n", passwords[i]);
    // }
    printf("total map size: %i\n", map_size);

    printf("\n###   ###   ###   ###   ###\n\n");
    printf("salt: %s\n", salt);
    printf("successes: %i\nfailures: %i\ntotal: %i\n", success_counter, fail_counter, success_counter + fail_counter);


    free(line);

    fclose(passwd);
    fclose(shadow);

    return 0;
}