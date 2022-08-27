#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char **argv)
{
    int size;
    int rank;
    int tag;
    int chunksize;
    int letters[26];
    int start;
    int lineCount;
    int result[26];
    int Addletters[26];
    unsigned char c;
    char *data;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0)
    {
        FILE *file;
        file = fopen("WarAndPeace.txt", "r");
        while ((c = getc(file)) != EOF)
        {
            if (isalpha(c))
            {
                lineCount++;
            }
        }
        fclose(file);
        data = (char *)malloc(sizeof(char) * lineCount);
        chunksize = lineCount / size;
        int remainder = lineCount % size;

        file = fopen("WarAndPeace.txt", "r");
        int n = 0;
        while ((c = getc(file)) != EOF)
        {
            if (isalpha(c))
            {
                data[n] = tolower(c);
                n++;
            }
        }
        fclose(file);
        for (int i = 1; i < size; i++)
        {
            start = i * chunksize;
            MPI_Send(&(data[start]), chunksize, MPI_BYTE, i, tag, MPI_COMM_WORLD);
            if (i == (size - 1))
            {
                chunksize + remainder;
            }
        }
        for (int i = 1; i < size; i++)
        {
            MPI_Recv(&result, 1, MPI_BYTE, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD,
                     MPI_STATUS_IGNORE);

            for (int i = 0; i < 26; i++)
            {
                Addletters[i] += result[i];
            }
        }
        for (int c = 0; c < 26; c++)
        {
            printf("%c \t %d\n", c + 'a', Addletters[c]);
        }
    }
    else
    {
        MPI_Recv(&data, chunksize, MPI_BYTE, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for (int i = 0; i < chunksize; i++)
        {
            if (data[i] >= 'a' && data[i] <= 'z')
            {
                letters[data[i] - 'a']++;
            }
        }
        MPI_Send(&letters, 1, MPI_BYTE, 0, tag, MPI_COMM_WORLD);
    }
    MPI_Finalize();
    return 0;
}