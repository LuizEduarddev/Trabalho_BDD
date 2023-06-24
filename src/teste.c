#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#define LEN 4096
void main()
{
    // Definindo o vetor de strings
    char vetorStrings[3][20]; // 3 strings com no máximo 20 caracteres cada
    
    // Preenchendo o vetor de strings
    strcpy(vetorStrings[0], "Primeira");
    strcpy(vetorStrings[1], "Segunda");
    strcpy(vetorStrings[2], "Terceira");
    
    // Juntando as strings em uma única string
    char stringFinal[100]; // String final para armazenar todas as strings
    for (int i = 0; i < 3; i++) {
        strcat(stringFinal, vetorStrings[i]);
        strcat(stringFinal, ",");
    }
    
    // Exibindo a string final
    printf("String final: %s\n", stringFinal);
    
    return 0;
    /*
    char arrayQuery[40][LEN];
    int op = 0;

    while (op != 1)
    {
        
        for(int i=0; i < 40; i++)
        {
            fprintf(stdout, "Digite uma Query: ");
            gets(arrayQuery[i]);
            fprintf(stdout, "Adicionar mais?\n1 - Sim\n2 - Nao\n: ");
            scanf("%d", &op);
            if (op == 2)
            {
                op = 1;
                break;
            }
        }

    }
    */
}