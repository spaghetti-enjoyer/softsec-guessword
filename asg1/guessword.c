#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <unistd.h>


int global_success_counter = 0;
int computation_counter = 0;
#define SHADOW_SIZE 10000

#define RAINBOW_SIZE 2000200
#define ENTRY_SIZE 64


int look_up(char *hash, char ***shadow_map, int map_size)
{
    for (int i = 0; i < map_size; i++)
    {
        if (strcmp(hash, (*shadow_map)[i]) == 0)
        {
            return i;
        }
    }
    return -1;
}

int insert_shadow_passwd(char *username, char *shadow_password, char ***username_map, char ***shadow_map, bool **present_items, int *map_size)
{
    strcpy((*username_map)[*map_size], username);
    strcpy((*shadow_map)[*map_size], shadow_password);
    (*present_items)[*map_size] = false;
    (*map_size)++;
    return 0;
}

// void print_map(char **usernames, char **passwords, bool *present_items, int map_size)
// {
//     for (int i = 0; i < map_size; i++)
//     {
//         printf("%s : %s : guessed? %s\n", usernames[i], passwords[i], present_items[i] ? "true" : "false");
//     }
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







int process_shadow_line(char *line, int len, char **username, char **password)
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
        return -1; // Error: Invalid line format
    }

    *username = malloc((username_end + 1) * sizeof(char));
    *password = malloc((password_end - password_start + 1) * sizeof(char));

    if (*username == NULL || *password == NULL) {
        return -1; // Error: Memory allocation failed
    }

    strncpy(*username, line, username_end);
    (*username)[username_end] = '\0';

    strncpy(*password, line + password_start, password_end - password_start);
    (*password)[password_end - password_start] = '\0';

    return 0;
}







int get_all_names_from_line(char ***all_names, char *line)
{
    int len = strlen(line);

    int start_index = -1;
    int end_index = -1;
    int colon_counter = 0;
    // int comma_counter = 0;

    for (int i = 0; i < len; i++)
    {
        if (line[i] == ':')
        {
            colon_counter++;
            // printf("colon! %i\n", i);
        }
        if (colon_counter == 4 && start_index == -1)
        {
            start_index = i + 1;
        }
        if (colon_counter == 4 && line[i] == ',')
        {
            // printf("comma! %i\n", i);
            end_index = i-1;
            break; 
        }
    }


    if (start_index == -1 || end_index == -1)// || start_index >= end_index)
    {
        printf("invalid indeces from supplied etc/passwd line: %s\n", line);
        return -1; 
    }

    int names_string_size = end_index - start_index + 1;
    char *names_string = malloc((names_string_size + 1) * sizeof(char));
    strncpy(names_string, line + start_index, names_string_size);
    names_string[names_string_size] = '\0';

    if (names_string == NULL)
    {
        printf("unable to malloc for names_string.\n");
        return -1;
    }


    int space_counter = 0;
    for (int i = 0; i < names_string_size; i++)
    {
        if (names_string[i] == ' ')
        {
            space_counter++;
        }
    }

    int names_count = space_counter + 1;
    *all_names = malloc(names_count * sizeof(char *));
    if (all_names == NULL)
    {
        printf("unable to malloc for all names.\n");
        return -1;
    }


    int name_start = 0;
    int name_index = 0;
    for (int i = 0; i < names_string_size + 1; i++)
    {
        if (names_string[i] == ' ' || names_string[i] == '\0')
        {
            int name_length = i - name_start;
            (*all_names)[name_index] = malloc((name_length + 1) * sizeof(char));
            if ((*all_names)[name_index] == NULL)
            {
                printf("unable to malloc for a name.\n");
                for (int j = 0; j < name_index; j++)
                {
                    free((*all_names)[j]);
                }
                free(*all_names);
                free(names_string);
                return -1;
            }
            strncpy((*all_names)[name_index], names_string + name_start, name_length);
            (*all_names)[name_index][name_length] = '\0';

            // make lowercase
            (*all_names)[name_index][0] += 32;

            name_index++;
            name_start = i + 1;
        }
    }

    // printf("string: %s\n", names_string);
    // printf("name count: %i\n", names_count);

    // for (int i = 0; i < names_count; i++)
    // {
    //     printf("name %i: %s\n", i, (*all_names)[i]);
    // }

    free(names_string);
    return names_count;
}







char *make_password_with_number(char *name, int number)
{
    int name_len = strlen(name);
    int number_len = snprintf(NULL, 0, "%d", number);

    char *result = malloc((name_len + number_len + 1) * sizeof(char));
    if (result == NULL)
    {
        return NULL;
    }

    sprintf(result, "%s%d", name, number);
    return result;
}

char *make_password_from_caps(char *name)
{
    int name_len = strlen(name);
    char *result = malloc(name_len * sizeof(char));
    if (result == NULL)
    {
        return NULL;
    }

    for (int i = 0; i < name_len; i++)
    {
        result[i] = name[i] - 32;
    }

    return result;
}

char *make_password_with_cap(char *name, int i)
{
    int name_len = strlen(name);
    if (i >= name_len)
    {
        return NULL;
    }

    char *result = malloc(name_len * sizeof(char));
    if (result == NULL)
    {
        return NULL;
    }

    strcpy(result, name);
    result[i] = result[i] - 32; // to capital

    return result;
}

char *insert_funny_letter(char *name, char *funny_letter, int index)
{
    int name_len = strlen(name);
    int letter_len = strlen(funny_letter);

    char *result = malloc((name_len + letter_len + 1) * sizeof(char));
    if (result == NULL)
    {
        return NULL;
    }

    // strncpy(result, name, index);
    // strncpy(result + index, funny_letter, letter_len);
    // strncpy(result + index + letter_len, name + index + 1, name_len - index - 1);

    memcpy(result, name, index);
    memcpy(result + index, funny_letter, letter_len);
    memcpy(result + index + letter_len, name + index + 1, name_len - index - 1);
    
    result[name_len + letter_len] = '\0';

    return result;
}

char *make_password_with_funny_letter(char *name, int index)
{
    char letter = name[index];

    if (letter == 'w')
    {
        char *funny_letter = "\\/\\/";
        return insert_funny_letter(name, funny_letter, index);
    }
    else if (letter == 't')
    {
        char *funny_letter = "-|-";
        return insert_funny_letter(name, funny_letter, index);

    }
    else if (letter == 'h')
    {
        char *funny_letter = "}{";
        return insert_funny_letter(name, funny_letter, index);
    }
    else if (letter == 'm')
    {
        char *funny_letter = "|\\/|";
        return insert_funny_letter(name, funny_letter, index);
    }
    else if (letter == 'c')
    {
        char *funny_letter = "(";
        return insert_funny_letter(name, funny_letter, index);
    }
    else if (letter == 'k')
    {
        char *funny_letter = "|(";
        return insert_funny_letter(name, funny_letter, index);
    }
    else if (letter == 'p')
    {
        char *funny_letter = "|*";
        return insert_funny_letter(name, funny_letter, index);
    }
    else if (letter == 'a')
    {
        char *funny_letter = "4";
        return insert_funny_letter(name, funny_letter, index);
    }
    else if (letter == 'e')
    {
        char *funny_letter = "3";
        return insert_funny_letter(name, funny_letter, index);
    }
    else if (letter == 'o')
    {
        char *funny_letter = "0";
        return insert_funny_letter(name, funny_letter, index);
    }
    else if (letter == 's')
    {
        char *funny_letter = "5";
        return insert_funny_letter(name, funny_letter, index);
    }
    else if (letter == 'v')
    {
        char *funny_letter = "^";
        return insert_funny_letter(name, funny_letter, index);
    }
    else if (letter == 'b')
    {
        char *funny_letter = "8";
        return insert_funny_letter(name, funny_letter, index);
    }
    else if (letter == 'u')
    {
        char *funny_letter = "L|";
        return insert_funny_letter(name, funny_letter, index);
    }
    else if (letter == 'y')
    {
        char *funny_letter = "`/";
        return insert_funny_letter(name, funny_letter, index);
    }
    return NULL;
}

char *make_password_with_suffix(char *name, char *suffix)
{
    int name_len = strlen(name);
    int suffix_len = strlen(suffix);

    char *result = malloc((name_len + suffix_len + 1) * sizeof(char));
    if (result == NULL)
    {
        return NULL;
    }
    sprintf(result, "%s%s", name, suffix);
    return result;
}









void try_password(char *password, char *salt, char *username, char *hash, bool **guessed, int index)
{
    if ((*guessed)[index] == true)
    {
        return;
    }

    char *res = crypt(password, salt);
    computation_counter++;

    if (strcmp(res, hash) == 0)
    {
        printf("%s:%s\n", username, password);
        fflush(stdout);
        (*guessed)[index] = true;
        global_success_counter++;
    }
    // printf("%s\n", password);
}









void insert_rainbow_into_map(char *file_name, char ***plain, char ***hashes, int *map_size, char *salt)
{
    FILE *input = fopen(file_name, "r");
    if (!input)
    {
        printf("unable to open rainbow file %s.\n", file_name);
        return;
    }

    char common_password[ENTRY_SIZE];
    while (fscanf(input, "%s", common_password) == 1)
    {
        // printf("read password %i: %s\n", *map_size, common_password);
        
        char *common_hash = crypt(common_password, salt);
        computation_counter++;

        // insert_hash(common_password, hash);
        strcpy((*plain)[*map_size], common_password);
        strcpy((*hashes)[*map_size], common_hash);
        (*map_size)++;
    }
    
    fclose(input);
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


    int shadow_password_map_size = 0;

    char **passwd_usernames = malloc(SHADOW_SIZE * sizeof(char *));
    char **shadow_passwords = malloc(SHADOW_SIZE * sizeof(char *));
    bool *shadow_passwords_guessed = malloc(SHADOW_SIZE * sizeof(bool));

    if (shadow_passwords == NULL || shadow_passwords_guessed == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }


    // Initialize the arrays
    for (int i = 0; i < SHADOW_SIZE; i++)
    {
        passwd_usernames[i] = malloc(ENTRY_SIZE * sizeof(char));
        if (passwd_usernames[i] == NULL)
        {
            printf("Memory allocation failed\n");
            return 1;
        }
        shadow_passwords[i] = malloc(ENTRY_SIZE * sizeof(char));
        if (shadow_passwords[i] == NULL)
        {
            printf("Memory allocation failed\n");
            return 1;
        }
        shadow_passwords_guessed[i] = false;
    }


    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    // read /etc/shadow and try combinations with real names of users
    char *salt = NULL;
    while ((read = getline(&line, &len, shadow)) != -1)
    {
        // obtaining the salt
        if (salt == NULL)
        {
            salt = get_salt(line, read);
        }

        // insert into the unknown passwords from shadow
        char *username = NULL;
        char *hashed_password = NULL;
        int err = process_shadow_line(line, read, &username, &hashed_password);

        if (err != 0)
        {
            printf("Problems with processing /etc/shadow.\n");
            continue;
        }

        // printf("%s: %s\n", username, hashed_password);
        insert_shadow_passwd(username, hashed_password, &passwd_usernames, &shadow_passwords, &shadow_passwords_guessed, &shadow_password_map_size);


        // ########################################
        // continue;
        // ########################################


        // getting the names from /etc/passwd
        char *passwd_line = NULL;
        size_t passwd_len = 0;
        ssize_t passwd_read;

        if ((passwd_read = getline(&passwd_line, &passwd_len, passwd)) == -1)
        {
            // did not read line from /etc/passwd for some reason
            printf("could not read from etc/passwd.\n");
            continue;
        }

        // check if usernames are same => lines in correct order
        char user_from_shadow[7];
        char user_from_passwd[7];
        strncpy(user_from_shadow, line, 6);
        user_from_shadow[6] = '\0';
        strncpy(user_from_passwd, passwd_line, 6);
        user_from_passwd[6] = '\0';

        if (strcmp(user_from_passwd, user_from_shadow) != 0)
        {
            printf("lines differ!\n");
            // continue;
            return 1;
        }


        char **real_names = NULL;
        int names_count = get_all_names_from_line(&real_names, passwd_line);

        if (names_count < 1)
        {
            printf("invalid name count returned: %i\n", names_count);
            continue;
        }


        // printf("%s", passwd_line);
        for (int i = 0; i < names_count; i++)
        {
            // printf("%i: %s\n", i, real_names[i]);

            // try name
            char *proposed_password = real_names[i];
            try_password(proposed_password, salt, username, hashed_password, &shadow_passwords_guessed, shadow_password_map_size);


            // try birth year
            for (int j = 1960; j < 1999; j++)
            {
                proposed_password = make_password_with_number(real_names[i], j);
                try_password(proposed_password, salt, username, hashed_password, &shadow_passwords_guessed, shadow_password_map_size);
            }

            // try 2 digits
            for (int j = 60; j < 99; j++) // from 0?
            {
                proposed_password = make_password_with_number(real_names[i], j);
                try_password(proposed_password, salt, username, hashed_password, &shadow_passwords_guessed, shadow_password_map_size);
            }

            // try all caps
            proposed_password = make_password_from_caps(real_names[i]);
            try_password(proposed_password, salt, username, hashed_password, &shadow_passwords_guessed, shadow_password_map_size);


            // try 1 capital
            int name_len = strlen(real_names[i]);
            for (int j = 0; j < name_len; j++)
            {
                proposed_password = make_password_with_cap(real_names[i], j);
                if (proposed_password == NULL)
                {
                    continue;
                }
                try_password(proposed_password, salt, username, hashed_password, &shadow_passwords_guessed, shadow_password_map_size);

            }

            // try funky letters
            for (int j = 0; j < name_len; j++)
            {
                proposed_password = make_password_with_funny_letter(real_names[i], j);
                if (proposed_password == NULL)
                {
                    continue;
                }
                try_password(proposed_password, salt, username, hashed_password, &shadow_passwords_guessed, shadow_password_map_size);

            }

            // try xor
            proposed_password = make_password_with_suffix(real_names[i], "xor");
            try_password(proposed_password, salt, username, hashed_password, &shadow_passwords_guessed, shadow_password_map_size);


            // try zorz
            proposed_password = make_password_with_suffix(real_names[i], "zorz");
            try_password(proposed_password, salt, username, hashed_password, &shadow_passwords_guessed, shadow_password_map_size);


            free(proposed_password);
        }
        

        // freeing 
        free(username);
        free(hashed_password);
        free(passwd_line);

        for (int i = 0; i < names_count; i++)
        {
            free(real_names[i]);
        }
        free(real_names);
    }


    int rainbow_size = 0;

    char **plain_rainbow = malloc(RAINBOW_SIZE * sizeof(char *));
    char **hashed_rainbow = malloc(RAINBOW_SIZE * sizeof(char *));

    if (plain_rainbow == NULL || hashed_rainbow == NULL)
    {
        printf("failed to allocate memory for rainbow file contents.\n");
        return 1;
    }

    for (int i = 0; i < RAINBOW_SIZE; i++)
    {
        plain_rainbow[i] = malloc(ENTRY_SIZE * sizeof(char));
        if (plain_rainbow[i] == NULL)
        {
            printf("Memory allocation failed.\n");
            for (int j = 0; j < i; j++) 
            {
                free(plain_rainbow[j]);
            }
            free(plain_rainbow);
            free(hashed_rainbow);
            return 1;
        }
        hashed_rainbow[i] = malloc(ENTRY_SIZE * sizeof(char));
        if (hashed_rainbow[i] == NULL)
        {
            printf("Memory allocation failed.\n");
            for (int j = 0; j <= i; j++)
            {
                free(plain_rainbow[j]);
            }
            for (int j = 0; j < i; j++)
            {
                free(hashed_rainbow[j]);
            }
            free(plain_rainbow);
            free(hashed_rainbow);
            return 1;
        }
    }

    int rainbow_files = 11; // 12 total, 9 ok
    char *inputs[] = {
                    "top_250_raw.txt", 
                    "unique_words.txt", 
                    "all_caps.txt", 
                    "one_capital.txt", 
                    "funny_letters.txt",
                    "zorz.txt",
                    "xor.txt",
                    "birthdays.txt",
                    "two_word_combinations.txt",
                    "common_names.txt",
                    "names.txt"
                    };
                    // "up_to_million.txt",
                    // };


    // process files one-by-one

    for (int i = 0; i < rainbow_files; i++)
    {
        insert_rainbow_into_map(inputs[i], &plain_rainbow, &hashed_rainbow, &rainbow_size, salt);


        if (rainbow_size < 1)
        {
            printf("did not correctly process rainbow file %s.\n", inputs[i]);
            continue;
        }

        for (int j = 0; j < shadow_password_map_size; j++)
        {
            if (shadow_passwords_guessed[j] == true)
            {
                continue;
            }

            for (int k = 0; k < rainbow_size; k++)
            {
                if (strcmp(shadow_passwords[j], hashed_rainbow[k]) == 0)
                {
                    printf("%s:%s\n", passwd_usernames[j], plain_rainbow[k]);
                    fflush(stdout);
                    shadow_passwords_guessed[j] = true;
                    global_success_counter++;
                }
            }
        }
        rainbow_size = 0;
    }



    // printf("\n###   ###   ###   ###   ###\n\n");
    // printf("salt: %s\n", salt);
    // printf("guessed: %i\n", global_success_counter);
    // printf("total hashes computed: %i\n", computation_counter);
    // printf("successes: %i\nfailures: %i\ntotal: %i\n", success_counter, fail_counter, success_counter + fail_counter);


    free(line);

    for (int i = 0; i < RAINBOW_SIZE; i++)
    {
        free(plain_rainbow[i]);
        free(hashed_rainbow[i]);
    }
    free(plain_rainbow);
    free(hashed_rainbow);

    fclose(passwd);
    fclose(shadow);

    return 0;
}