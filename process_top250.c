#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *file = fopen("dictionary/top250.txt", "r");
    if (file == NULL) {
        perror("Failed to open file");
        return EXIT_FAILURE;
    }

    FILE *output = fopen("top_250_raw.txt", "w");
    if (!output)
    {
        printf("failed to create output fle.\n");
        return 1;
    }

    int rank, count;
    float percentage;
    char password[50];

    while (fscanf(file, "%d %f %d %s", &rank, &percentage, &count, password) == 4) {
        printf("Rank: %d, Percentage: %.2f, Count: %d, Password: %s\n", rank, percentage, count, password);
        fprintf(output, "%s\n", password);
    }

    fclose(file);
    fclose(output);
    return EXIT_SUCCESS;
}