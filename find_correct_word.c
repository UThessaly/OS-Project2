#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[])
{
    FILE *fp;
    char words[50][20];
    char check_word[20];
    int num_words = 0, i = 0, flag, j, k;

    if(argc != 3)
    {
        printf("Wrong number of arguments..\n");
        return 1;
    }

    strcpy(check_word, argv[2]);
    
    fp = fopen(argv[1], "r");

    while(!feof(fp))
    {
        fscanf(fp, "%s", words[i]);
        i++;
        num_words++;
    }


    for(i=0; i<num_words; i++)
    {
        if(strlen(words[i]) == strlen(check_word))
        {
            flag = 0;
            for(j=0; j<strlen(check_word); j++)
            {
                for(k=0; k<strlen(words[i]); k++)
                {
                    if(words[i][k] == check_word[j])
                    {
                        flag++;
                    }

                }
                if(flag == strlen(check_word))
                {
                    printf("%s", words[i]);
                    break;
                }
            }
        }
    }


    return 0;
}