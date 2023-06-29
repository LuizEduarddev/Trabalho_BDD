#include "/usr/include/postgresql/libpq-fe.h"
#include "../files/functions.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<ctype.h>
/*Definicoes para facilitar os retornos de funcao*/
#define ERRO -1
#define SUCESSO 0
#define LEN 4096
#define INTEGER 1
#define NUMERIC 2
#define VAARCHAR 3
#define DATE 4
#define TIMESTAMP 5
#define BOOLEAN 6
#define DIFERENT 7
/*Funcao para fechar a conexao com o servidor*/
void close_connection(PGconn *conn)
{
    PQfinish(conn);
    exit(0);
}

void insert_intodb(PGconn *conn)
{
    PGresult *resultado;

    resultado = PQexec(conn, "SELECT table_name FROM information_schema.tables WHERE table_schema='public'");

    if (PQresultStatus(resultado) != PGRES_TUPLES_OK) 
    {
        fprintf(stderr, "Falha na execucao da Query: %s\n", PQerrorMessage(conn));
        PQclear(resultado);
        close_connection(conn);
    }

    tabelasDB(conn);

    char nomeTab[LEN];
    int erro = SUCESSO;

    __fpurge(stdin);
    fprintf(stdout, "Qual tabela deseja inserir um valor? ");
    scanf("%s%*c", nomeTab);

    __fpurge(stdin);
    erro = isinDB(conn, nomeTab);
    while (erro == ERRO)
    {
        __fpurge(stdin);
        fprintf(stdout, "A tabela digitada nao parece estar em nosso banco de dados\nPor favor, tente novamente: ");
        scanf("%s", nomeTab);
        erro = isinDB(conn, nomeTab);
    }
    
    /*
        Este clear serve para limpar o conteudo presente na variavel resultado
        Para que caso venha a ser usada novamente, nao ocorra nenhum tipo de erro
        como substituicao dos dados.
    */
    PQclear(resultado);

    int number = 0;
    char inserir_tabela[LEN];
    sprintf(inserir_tabela, "SELECT * FROM %s", nomeTab);

    resultado = PQexec(conn, inserir_tabela);
    
    /*Caso o resultado de SELECT seja um erro, finaliza a conexao*/
    if (PQresultStatus(resultado) != PGRES_TUPLES_OK)
    {
        fprintf(stderr, "Erro na busca da tabela.\nErro: %s", PQresultErrorMessage(resultado));
        PQclear(resultado);
        close_connection(conn);
    }

    int numCol = PQnfields(resultado);

    printf("Quantos dados deseja inserir na tabela?\n: ");
    scanf("%d", &number);
    for(int j=0 ; j < number; j++)
    {
        char nomeCol[LEN];
        char tipoCol[LEN];
        char Query[numCol][LEN];
        for (int i=0; i < numCol; i++)
        {
            __fpurge(stdin);
            printf("Coluna: %d\n", i+1);
            printf("Tipo da coluna: %s\n", pegaTipo(tipoCol, resultado, i));
            printf("Nome da coluna - %s\n", PQfname(resultado, i));

            strcpy(nomeCol, PQfname(resultado, i));
            funcType(conn, nomeCol, tipoCol, nomeTab);

            printf("Inserido com sucesso!\n");
            
        }
    }
}

char *pegaTipo(char *tipoCol, PGresult *resultado, int numCol)
{
    Oid tipoColuna = PQftype(resultado, numCol);

    if (tipoColuna == 23)
    {
        strcpy(tipoCol, "integer");
        return "integer";
    }
    else if(tipoColuna == 1700)
    {
        strcpy(tipoCol, "decimal");
        return "decimal";
    }
    else if(tipoColuna == 1043)
    {
        strcpy(tipoCol, "varchar");
        return "varchar";
    }
    else
    {
        return "Tipo nao detectado";
    }
}

void funcType(PGconn *conn, char *nomeCol, char *tipoCol, char *nomeTab)
{
    __fpurge(stdin);
    if (strcmp(tipoCol, "integer") == 0)
        GoToInteger(conn, nomeCol, tipoCol, nomeTab);
    else if(strcmp(tipoCol, "decimal") == 0 || strcmp(tipoCol, "numeric") == 0)
        GoToNumeric(conn,nomeCol, tipoCol, nomeTab);
    else if(strcmp(tipoCol, "varchar") == 0)
        GoToString(conn, nomeCol, tipoCol, nomeTab);
    else
    {
        fprintf(stdout, "Erro ao detectar o tipo da coluna.\n");
        close_connection(conn);
    }
}

void GoToInteger(PGconn *conn, char *nomeCol, char *tipoCol, char *nomeTab)
{
    PGresult *res;
    int valor;
    char consulta[LEN];

    __fpurge(stdin);
    fprintf(stdout, "Digite um valor inteiro para ser inserido na coluna '%s': ", nomeCol);
    scanf("%d", &valor);

    sprintf(consulta, "INSERT INTO %s (%s) VALUES (%d)", nomeTab, nomeCol, valor);

    res = PQexec(conn, consulta);

    if(PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        fprintf(stdout, "Erro ao tentar executar a acao.\nErro: %s", PQresultErrorMessage(res));
        close_connection(conn);
    }
}

void GoToNumeric(PGconn *conn, char *nomeCol, char *tipoCol, char *nomeTab)
{
    PGresult *res;
    float valor;
    char consulta[LEN];

    __fpurge(stdin);
    fprintf(stdout, "Digite um valor float para ser inserido na coluna '%s': ", nomeCol);
    scanf("%f", &valor);

    sprintf(consulta, "INSERT INTO %s (%s) VALUES (%f)", nomeTab, nomeCol, valor);

    res = PQexec(conn, consulta);

    if(PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        fprintf(stdout, "Erro ao tentar executar a acao.\nErro: %s", PQresultErrorMessage(res));
        close_connection(conn);
    }
}

void GoToString(PGconn *conn, char *nomeCol, char *tipoCol, char *nomeTab)
{
    PGresult *res;
    char valor[LEN];
    char consulta[LEN];

    __fpurge(stdin);
    fprintf(stdout, "Digite uma string para ser inserido na coluna '%s': ", nomeCol);
    gets(valor);

    sprintf(consulta, "INSERT INTO %s (%s) VALUES ('%s')", nomeTab, nomeCol, valor);

    res = PQexec(conn, consulta);

    if(PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        fprintf(stdout, "Erro ao tentar executar a acao.\nErro: %s", PQresultErrorMessage(res));
        close_connection(conn);
    }
}

void tabelasDB(PGconn *conn)
{
    PGresult *resultado;

    resultado = PQexec(conn, "SELECT table_name FROM information_schema.tables WHERE table_schema='public'");
    /*
        Esta variavel aqui faz o seguinte, pega a quantidade de tabelas baseado no retorno da funcao
        PQexec. 
    */
    int lencolunas = PQntuples(resultado);

    fprintf(stdout, "Tabelas presentes no DataBase.\n");

    /*Este for retorna o nome de todas as tabelas do DB*/
    for (int i=0; i < lencolunas; i++)
    {
        char *tableName = PQgetvalue(resultado, i, 0);
        printf("%d - %s\n", (i+1) ,tableName);
    }
}

int isinDB(PGconn *conn, char *nomeCol)
{
    PGresult *resultado;

    resultado = PQexec(conn, "SELECT table_name FROM information_schema.tables WHERE table_schema='public'");
    /*
        Esta variavel aqui faz o seguinte, pega a quantidade de tabelas baseado no retorno da funcao
        PQexec. 
    */

    int lencolunas = PQntuples(resultado);
    /*Este for retorna o nome de todas as tabelas do DB*/
    /*
        Caso a tabela digita pelo usuario nao existir no banco de dados
        o retorno da funcao sera -1
        caso ela exista, o retorno sera igual a 0.
    */
    for (int i=0; i < lencolunas; i++)
    {
        char *tableName = PQgetvalue(resultado, i, 0);
        if (strcmp(tableName, nomeCol) == 0)
        {
            return SUCESSO;
        }
    }
    return ERRO;
}

char *toUper(char *string)
{
    for (int i=0; string[i] != '\0'; i++)
    {
        string[i] = toupper(string[i]);
    }

    return string;
}

void create_table(PGconn *conn)
{
    PGresult *resultado;
    char nomeTab[LEN];
    char tipo[LEN];
    char consulta[LEN];
    int op = 0;

    getchar();

    fprintf(stdout, "Digite o nome da tabela a ser criada: ");
    fgets(nomeTab, LEN, stdin);
    
    getType(nomeTab, consulta);

    resultado = PQexec(conn, consulta);

    if (PQresultStatus(resultado) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Falha na criacao da tabela.\nErro: %s\n", PQerrorMessage(conn));
        close_connection(conn);
        return EXIT_FAILURE;
    }

    fprintf(stdout, "Inserido com sucesso na tabela.\n");
}

void *getType(char *nomeTab, char *result)
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

    sprintf(Query, "CREATE TABLE %s (%s)", nomeTab, consulta);
    strcpy(result, Query);
}

void specTable(PGconn *conn)
{
    PGresult *resultado;

    resultado = PQexec(conn, "SELECT table_name FROM information_schema.tables WHERE table_schema='public'");

    /*
        O que acontece neste if eh, caso ocorra uma falha na requisao das tabelas, ele retorna um erro e fecha o db
    */
    if (PQresultStatus(resultado) != PGRES_TUPLES_OK) 
    {
        fprintf(stderr, "Falha na execucao da Query: %s\n", PQerrorMessage(conn));
        PQclear(resultado);
        close_connection(conn);
    }

    tabelasDB(conn);

    char tipoCol[LEN];
    char nomeTab[LEN];
    int erro = SUCESSO;
    __fpurge(stdin);
    fprintf(stdout, "Qual tabela deseja ver os campos e tipos de dado?\n: ");
    gets(nomeTab);

    erro = isinDB(conn, nomeTab);
    while (erro == ERRO)
    {
        __fpurge(stdin);
        fprintf(stdout, "A tabela digitada nao parece estar em nosso banco de dados\nPor favor, tente novamente: ");
        gets(nomeTab);
        erro = isinDB(conn, nomeTab);
    }

    /*
        Este clear serve para limpar o conteudo presente na variavel resultado
        Para que caso venha a ser usada novamente, nao ocorra nenhum tipo de erro
        como substituicao dos dados.
    */
    PQclear(resultado);

    char consulta[100];
    sprintf(consulta, "SELECT * FROM %s", nomeTab);
    resultado = PQexec(conn, consulta);
    if (PQresultStatus(resultado) != PGRES_TUPLES_OK) 
    {
        fprintf(stderr, "Falha ao executar a consulta: %s\n", PQerrorMessage(conn));
        PQclear(resultado);
        PQfinish(conn);
        return;
    }

    int Ncolunas  = PQntuples(resultado);  
    int Nlinhas = PQnfields(resultado);

    if (Ncolunas == 0)
    {
        fprintf(stdout, "Parece que ainda nao existem dados nesta tabela, insira novos dados e tente novamente.\n");

        for(int n=0; n < Nlinhas; n++)
        {
            printf("Coluna - %d\n", n + 1);
            printf("Tipo - %s\n", pegaTipo(tipoCol, resultado, n));
            printf("Nome da coluna: %s\n", PQfname(resultado, n));
        }
    }
    
    
    printf("Valor de Ncolunas:%d\nValor de Nlinhas: %d", Ncolunas, Nlinhas);


    PQclear(resultado);
    return 0;
}
void showData(PGconn *conn)
{
    PGresult *resultado;

    resultado = PQexec(conn, "SELECT table_name FROM information_schema.tables WHERE table_schema='public'");

    /*
        O que acontece neste if eh, caso ocorra uma falha na requisao das tabelas, ele retorna um erro e fecha o db
    */
    tryError(conn, resultado);

    tabelasDB(conn);

    char nomeTab[LEN];
    char nomeCol[LEN];
    char typeCol[LEN];
    char inserir_tabela[LEN];
    char filter[LEN];
    int erro = SUCESSO;
    __fpurge(stdin);
    fprintf(stdout, "Qual tabela deseja ver para filtar?\n: ");
    gets(nomeTab);

    erro = isinDB(conn, nomeTab);
    while (erro == ERRO)
    {
        __fpurge(stdin);
        fprintf(stdout, "A tabela digitada nao parece estar em nosso banco de dados\nPor favor, tente novamente: ");
        gets(nomeTab);
        erro = isinDB(conn, nomeTab);
    }

    /*
        Este clear serve para limpar o conteudo presente na variavel resultado
        Para que caso venha a ser usada novamente, nao ocorra nenhum tipo de erro
        como substituicao dos dados.
    */
    PQclear(resultado);

    fprintf(stdout, "OK!\nAgora precisamos filtar.\nA filtragem é simples, primeiro escolha uma coluna, a tabela gerada será baseada nesta coluna.\nEX: coluna_nome = joao. O resultado sera uma tabela com todos os nomes joao.");
    
    sprintf(inserir_tabela, "SELECT * FROM %s", nomeTab);
    resultado = PQexec(conn, inserir_tabela);
    tryError(conn, resultado);

    __fpurge(stdin);
    fprintf(stdout, "Digite o nome da coluna: ");
    gets(nomeCol);

    __fpurge(stdin);
    fprintf(stdout, "Certo, agora escolha um valor para filtrar\nOBS: O valor pode ser um nome, uma string ou até mesmo um número float ou inteiro\n: ");
    gets(filter);

    __fpurge(stdin);
    fprintf(stdout, "Perfeito, agora como deve ser feita esta filtragem?\nEscolha algum dos itens do menu\n1 - Maior\n2 - Maior ou igual\n3 - Menor\n4 - Menor ou igual\n5 - Diferente de\n6 - Igual\n");
    strcpy(inserir_tabela, cathSwitch(nomeCol, filter, nomeTab)); 

    resultado = PQexec(conn, inserir_tabela);
    
    /*Caso o resultado de SELECT seja um erro, finaliza a conexao*/
    if (PQresultStatus(resultado) != PGRES_TUPLES_OK)
    {
        fprintf(stderr, "Erro na busca da tabela.\nErro: %s", PQresultErrorMessage(resultado));
        PQclear(resultado);
        close_connection(conn);
    }
}

void tryError(PGconn *conn, PGresult *resultado)
{
    if (PQresultStatus(resultado) != PGRES_TUPLES_OK) 
    {
        fprintf(stderr, "Falha na execucao da Query: %s\n", PQerrorMessage(conn));
        PQclear(resultado);
        close_connection(conn);
    }
}

char *cathSwitch(char *nomeCol, char *filter, char *nomeTab)
{
    int op = 0;
    char retorno[LEN];
    //1 - Maior\n2 - Maior ou igual\n3 - Menor\n4 - Menor ou igual\n5 - Diferente de\n6 - Igual
    printf(": ");
    scanf("%d", &op);

    switch(op)
    {
        case 1:
        sprintf(retorno, "SELECT * FROM %s WHERE %s > %s", nomeTab, nomeCol, filter);
        return retorno;
        break;

        case 2:
        sprintf(retorno, "SELECT * FROM %s WHERE %s >= %s", nomeTab, nomeCol, filter);
        return retorno;
        break;

        case 3:
        sprintf(retorno, "SELECT * FROM %s WHERE %s < %s", nomeTab, nomeCol, filter);
        return retorno;
        break;

        case 4:
        sprintf(retorno, "SELECT * FROM %s WHERE %s <= %s", nomeTab, nomeCol, filter);
        return retorno;
        break;

        case 5:
        sprintf(retorno, "SELECT * FROM %s WHERE %s != %s", nomeTab, nomeCol, filter);
        return retorno;
        break;

        case 6:
        sprintf(retorno, "SELECT * FROM %s WHERE %s = %s", nomeTab, nomeCol, filter);
        return retorno;
        break;
    }
}

void rmData(PGconn *conn)
{
    PGresult *resultado;

    resultado = PQexec(conn, "SELECT table_name FROM information_schema.tables WHERE table_schema='public'");

    /*
        O que acontece neste if eh, caso ocorra uma falha na requisao das tabelas, ele retorna um erro e fecha o db
    */
    tryError(conn, resultado);

    tabelasDB(conn);

    char nomeTab[LEN];
    char nomeCol[LEN];
    char typeCol[LEN];
    char inserir_tabela[LEN];
    char filter[LEN];
    int erro = SUCESSO;
    int op = 0;

    __fpurge(stdin);
    fprintf(stdout, "Estas sao as tabelas disponiveis, deseja apagar uma tabela, ou um item de uma tabela?1 - Apagar a tabela\n2 - Apagar um item da tabela\n: ");
    scanf("%d", &op);

    if (op == 1)
    {
        rmTabela(conn);
        return;
    }
    else if(op == 2)
    {
        rmItem(conn);
        return;
    }
    else
    {
        printf("Fora da lista.\n");
        return;
    }
}

void rmTabela(PGconn *conn)
{
    PGresult *resultado;
    char nomeTab[LEN];
    char execucao[LEN];
    int erro = SUCESSO;
    int op = 0;

    __fpurge(stdin);
    fprintf(stdout, "Digite o nome da tabela que deseja remover.\n: ");
    gets(nomeTab);

    erro = isinDB(conn, nomeTab);
    while (erro == ERRO)
    {
        __fpurge(stdin);
        fprintf(stdout, "A tabela digitada nao parece estar em nosso banco de dados\nPor favor, tente novamente: ");
        gets(nomeTab);
        erro = isinDB(conn, nomeTab);
    }


    __fpurge(stdin);
    fprintf(stdout, "Ao fazer isto, TODOS os dados de sua tabela serao apagados, tem certeza que quer fazer isto?\n1 - Sim, sei de todos os problemas que isto pode causar\n2 - Nao, nao quero fazer isto\n: ");
    scanf("%d", &op);

    sprintf(execucao, "DROP TABLE %s", nomeTab);

    resultado = PQexec(conn, execucao);

    tryError(conn, resultado);
    return;
}

void rmItem(PGconn *conn)
{
    PGresult *resultado;
    char nomeTab[LEN];
    char search[LEN];
    char filtro[LEN];
    char nomeCol[LEN];
    int erro = SUCESSO;

    __fpurge(stdin);
    fprintf(stdout, "Certo, e de qual tabela deseja apagar algum item?\n: ");
    gets(nomeTab);

    erro = isinDB(conn, nomeTab);
    while (erro == ERRO)
    {
        __fpurge(stdin);
        fprintf(stdout, "A tabela digitada nao parece estar em nosso banco de dados\nPor favor, tente novamente: ");
        gets(nomeTab);
        erro = isinDB(conn, nomeTab);
    }

    sprintf(search, "SELECT * FROM %s", nomeTab);

    resultado = PQexec(conn, search);
    
    __fpurge(stdin);
    fprintf(stdout, "OK!\nAgora digite o valor que desejar deletar\nEX: nome = joao (todos os joao serao apagados da coluna nome.)\n: ");
    gets(filtro);

    __fpurge(stdin);
    fprintf(stdout, "PERFEITO!\nAgora digite o nome da coluna que tera o item deletado.\n: ");
    gets(nomeCol);

    fprintf(stdout, "So mais uma coisinha, digite o tipo da exclusao\n1 - Maior\n2 - Maior ou igual\n3 - Menor\n4 - Menor ou igual\n5 - Diferente de\n6 - Igual");

    switchItem(conn, nomeTab, filtro, nomeCol);
}

void switchItem(PGconn *conn, char *nomeTab, char *filter, char *nomeCol)
{
    int op = 0;
    char retorno[LEN];
    
    __fpurge(stdin);
    printf(": ");
    scanf("%d", &op);

    switch(op)
    {
        case 1:
        sprintf(retorno, "DELETE FROM %s WHERE %s > %s", nomeTab, nomeCol, filter);
        return retorno;
        break;

        case 2:
        sprintf(retorno, "DELETE FROM %s WHERE %s >= %s", nomeTab, nomeCol, filter);
        return retorno;
        break;

        case 3:
        sprintf(retorno, "DELETE FROM %s WHERE %s < %s", nomeTab, nomeCol, filter);
        return retorno;
        break;

        case 4:
        sprintf(retorno, "DELETE FROM %s WHERE %s <= %s", nomeTab, nomeCol, filter);
        return retorno;
        break;

        case 5:
        sprintf(retorno, "DELETE FROM %s WHERE %s != %s", nomeTab, nomeCol, filter);
        return retorno;
        break;

        case 6:
        sprintf(retorno, "DELETE FROM %s WHERE %s = %s", nomeTab, nomeCol, filter);
        return retorno;
        break;
    }
}