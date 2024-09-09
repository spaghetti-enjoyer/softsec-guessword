#include<stdio.h>
#include<stdlib.h>

#include<string.h>
#include<assert.h>


// https://www.geeksforgeeks.org/implementation-on-map-or-dictionary-data-structure-in-c/
#define MAX_SIZE 10000 // Maximum number of elements in the map 
  
int size = 0; // Current number of elements in the map 
char keys[MAX_SIZE][100]; // Array to store the keys 
int values[MAX_SIZE]; // Array to store the values 
  
// Function to get the index of a key in the keys array 
int getIndex(char key[]) 
{ 
    for (int i = 0; i < size; i++) { 
        if (strcmp(keys[i], key) == 0) { 
            return i; 
        } 
    } 
    return -1; // Key not found 
} 
  
// Function to insert a key-value pair into the map 
void insert(char key[]) 
{ 
    int value = 1;
    int index = getIndex(key); 

    if (index == -1) { // Key not found 
        strcpy(keys[size], key); 
        values[size] = value; 
        size++; 
    } 
    else { // Key found 
        values[index]++; 
    } 
} 
  
// // Function to get the value of a key in the map 
// int get(char key[]) 
// { 
//     int index = getIndex(key); 
//     if (index == -1) { // Key not found 
//         return -1; 
//     } 
//     else { // Key found 
//         return values[index]; 
//     } 
// } 

void swap(int i, int j)
{
    int temp = values[i];
    char buf[100];
    strcpy(buf, keys[i]);

    values[i] = values[j];
    strcpy(keys[i], keys[j]);

    values[j] = temp;
    strcpy(keys[j], buf);
}

void sort()
{
    for (int i = 0; i < size-1; i++)
    {
        for (int j = i+1; j < size; j++)
        {
            if (values[i] < values[j])
            {
                swap(i, j);
            }
        }
    }
}
  
// Function to print the map 
void saveMap() 
{
    FILE *output = NULL;
    output = fopen("common_password_count.txt", "w");

    for (int i = 0; i < size; i++) { 
        fprintf(output, "%s\t\t: %d\n", keys[i], values[i]); 
    } 

    fclose(output);
} 


char *get_password_from_plain(char *line, size_t len)
{
    int trailing_chars = 1;
    int password_start;

    for (int i = 0; i < len-1; i++)
    {
        if (line[i] == ':')
        {
            password_start = i+1; // next char
            break;
        }
    }

    int password_len = len - password_start - trailing_chars;
    char *password = malloc(password_len * sizeof(char));

    for (int i = 0; i < password_len; i++)
    {
        password[i] = line[i + password_start];
    }

    // printf("overall length: %zu -> %s -> %i\n", len,  password, password_len);

    return password;
}


int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Not enough arguments\n");
        return 1;
    }

    FILE *passwd = fopen(argv[1], "r");
    // FILE *shadow = fopen(argv[2], "r");

    if (!passwd)
    {
        printf("Could not open input file.\n");
        return 1;
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t number_read;

    while ((number_read = getline(&line, &len, passwd)) != -1)
    {
        printf("%s", line);

        char *password = get_password_from_plain(line, number_read);
        insert(password);
    }

    sort();
    saveMap();

    printf("ok!\n");

    fclose(passwd);
    return 0;
}