 #include<stdio.h>
 #include<stdlib.h>
 #define LEN 4096
 #define nomeTab "carros"
 void main()
 {
    char type[LEN];
    char consulta[LEN];
    char variavel[LEN];
    char arrayQuery[40][LEN];
    char nomeVariavel[LEN];
    char Query[LEN];
    int col, lenQuery = 0;

    __fpurge(stdin);
    fprintf(stdout, "Quantas colunas deseja adicionar? \n: ");
    scanf("%d", &col);

    for (int i=0 ; i < col; i++)
    {
        __fpurge(stdin);
        fprintf(stdout, "Digite o nome da coluna %d: ", i+1);
        fgets(nomeVariavel, sizeof(nomeVariavel), stdin);

        fprintf(stdout, "Tipos disponiveis de variaveis: \n");
        fprintf(stdout, "1 - INTEGER\n2 - DECIMAL\n3 - VARCHAR\n: ");
        __fpurge(stdin);
        fprintf(stdout, "Digite o tipo da variavel\nOBS: digite o nome do tipo e nao algum dos numeros mostrados acima.\n: ");
        fgets(type, sizeof(type), stdin);

        sprintf(arrayQuery[i], "%s %s", nomeVariavel, type);
        __fpurge(stdin);
    }
    
    for (int i=0; i < col; i++) 
    {
        strcat(consulta, arrayQuery[i]);
        strcat(consulta, ",");
    }
   
    lenQuery = strlen(consulta);
    consulta[lenQuery - 1] = '\0';

    fprintf(stdout, "String\n%s", consulta); 
    sprintf(Query, "CREATE TABLE %s (%s)", nomeTab, consulta);
    fprintf(stdout, "A Query eh:\n%s", Query);
 }