#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    FILE *output = fopen("up_to_million.txt", "w");
    if (!output)
    {
        printf("Error\n");
        return 1;
    }

    for (int i = 0; i <= 1000000; i++)
    {
        fprintf(output, "%i\n", i);
    }
    fclose(output);
}