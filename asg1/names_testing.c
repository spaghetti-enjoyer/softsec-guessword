#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <unistd.h>


int global_success_counter = 0;
#define SHADOW_SIZE 10000

// MAP IMPLEMENTATION

#define MAX_SIZE 10000000
// int map_size = 0;
// char passwords[MAX_SIZE][100]; 
// char hashes[MAX_SIZE][100];


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

// void insert_hash(char *new_password, char *new_hash)
// {
//     strcpy(passwords[map_size], new_password);
//     strcpy(hashes[map_size], new_hash);
//     map_size++;
// }

void print_map(char **usernames, char **passwords, bool *present_items, int map_size)
{
    for (int i = 0; i < map_size; i++)
    {
        printf("%s : %s : guessed? %s\n", usernames[i], passwords[i], present_items[i] ? "true" : "false");
    }
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


// char *unhash_password(char *encrypted_password)
// {
//     if (0) // timed out
//     {
//         return NULL;
//     }

//     int found = look_up(encrypted_password);

//     if (found == -1)
//     {
//         return NULL;
//     }

//     char *result = malloc((strlen(passwords[found]) + 1) * sizeof(char));
//     if (result == NULL)
//     {
//         printf("failed to allocate memory\n");
//         return NULL;
//     }

//     strcpy(result, passwords[found]);
//     return result;
// }


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


// void process_common_input_file(FILE *input, const char *salt)
// {
//     char common_password[100];
//     while (fscanf(input, "%99s", common_password) == 1)
//     {
//         // printf("%s\n", common_password);
//         char *hash = crypt(common_password, salt);
//         insert_hash(common_password, hash);

//     }
// }


// void read_all_common_inputs(char **file_names, int n, const char *salt)
// {
//     for (int i = 0; i < n; i++)
//     {
//         FILE *input = fopen(file_names[i], "r");
//         if (!input)
//         {
//             printf("Error opening rainbow file %s.\n", file_names[i]);
//             continue;
//         }
//         process_common_input_file(input, salt);
//         fclose(input);
//     }
// }






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

    strncpy(result, name, index);
    strncpy(result + index, funny_letter, letter_len);
    strncpy(result + index + letter_len, name + index + 1, name_len - index);
    result[name_len + letter_len] = '\0';

    return result;
}

char *make_password_with_funny_letter(char *name, int index)
{
    char letter = name[index];
    int name_len = strlen(name);
    // char *result = NULL;

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

    char *res = crypt(password, hash);
    if (strcmp(res, hash) == 0)
    {
        printf("%s:%s\n", username, password);
        (*guessed)[index] = true;
        global_success_counter++;
    }
    // printf("%s\n", password);
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

    char **passwd_usernames = malloc(MAX_SIZE * sizeof(char *));
    char **shadow_passwords = malloc(MAX_SIZE * sizeof(char *));
    bool *shadow_passwords_guessed = malloc(MAX_SIZE * sizeof(bool));

    if (shadow_passwords == NULL || shadow_passwords_guessed == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }

    // Initialize the arrays
    for (int i = 0; i < MAX_SIZE; i++)
    {
        passwd_usernames[i] = malloc(SHADOW_SIZE * sizeof(char));
        if (passwd_usernames[i] == NULL)
        {
            printf("Memory allocation failed\n");
            return 1;
        }
        shadow_passwords[i] = malloc(SHADOW_SIZE * sizeof(char));
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

        // printf("%s: %s\n", username, hashed_password);
        insert_shadow_passwd(username, hashed_password, &passwd_usernames, &shadow_passwords, &shadow_passwords_guessed, &shadow_password_map_size);



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
            for (int j = 0; j < 99; j++)
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

    // print_map(passwd_usernames, shadow_passwords, shadow_passwords_guessed, shadow_password_map_size);

    // // obtaining the salt
    // read = getline(&line, &len, shadow);
    // const char *salt = get_salt(line, read);


    // check passwords involving one's real name




    // // common passwords
    // int rainbow_files = 12;
    // char *inputs[] = {
    //                 "top_250_raw.txt", 
    //                 "unique_words.txt", 
    //                 "all_caps.txt", 
    //                 "one_capital.txt", 
    //                 "names.txt",
    //                 "common_names.txt",
    //                 "funny_letters.txt",
    //                 "zorz.txt",
    //                 "xor.txt",
    //                 "birthdays.txt",
    //                 "two_word_combinations.txt",
    //                 "up_to_million.txt"};

    // read_all_common_inputs(inputs, rainbow_files, salt);


    // int success_counter = 0;
    // int fail_counter = 0;
    // while ((read = getline(&line, &len, shadow)) != -1)
    // {
    //     // printf("%s", line);

    //     char *username = NULL;
    //     char *password = NULL;

    //     if (process_line(line, read, &username, &password) != 0)
    //     {
    //         fail_counter++;
    //         continue;
    //     }

    //     char *unhashed_password = unhash_password(password);

    //     if (unhashed_password == NULL)
    //     {
    //         fail_counter++;
    //         continue;
    //     }

    //     printf("%s:%s\n", username, unhashed_password);
    //     fflush(stdout);
    //     success_counter++;

    //     free(username);
    //     free(password);
    //     free(unhashed_password);
    // }

    // for (int i = 0; i < map_size; i++)
    // {
    //     printf("%s\n", passwords[i]);
    // }
    // printf("total map size: %i\n", map_size);

    printf("\n###   ###   ###   ###   ###\n\n");
    printf("salt: %s\n", salt);
    printf("guessed: %i\n", global_success_counter);
    // printf("successes: %i\nfailures: %i\ntotal: %i\n", success_counter, fail_counter, success_counter + fail_counter);


    free(line);

    fclose(passwd);
    fclose(shadow);

    return 0;
}