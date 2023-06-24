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
}

void insert_intodb(PGconn *conn)
{
    
    /*
        Para inserir no database pegamos a struct para a conexao com o db
        Em seguida pegamos a struct PQresult, que basicamente retorna os dados da tabela
        E com isso conseguimos manipular o banco de dados
    */

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

    char nomeTab[LEN];
    int erro = SUCESSO;
    fprintf(stdout, "Qual tabela deseja inserir um valor? ");
    gets(nomeTab);

    erro = isinDB(conn, nomeTab);
    while (erro == ERRO)
    {
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

    int op = -1;
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

    // Pega a quantidade de colunas da tabela
    int numCol = PQnfields(resultado);

    while (op != 0)
    {
        char nomeCol[30];
        char tipoCol[30];
        for (int i=0; i < numCol; i++)
        {
            printf("Coluna %d.\n", i);
            printf("Tipo da coluna: %s.\n", PQftypeName(conn, PQftype(resultado, i)));
    
            strcpy(nomeCol, PQgetvalue(resultado, i, 0));
            strcpy(tipoCol, PQftypeName(conn, PQftype(resultado, i)));

            funcType(conn, nomeCol, tipoCol, nomeTab);

            fprintf(stdout, "Inserido com sucesso.\n");
        }
    }
}


void funcType(PGconn *conn, char *nomeCol, char *tipoCol, char *nomeTab)
{
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

    fprintf(stdout, "Tabelas presentes no DataBase.\n");

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

    fprintf(stdout, "Digite o nome da tabela a ser criada");
    gets(nomeTab);

    strcpy(consulta, getType(nomeTab));
    

    resultado = PGexec(conn, consulta);

    if (PQresultStatus(resultado) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Falha na criacao da tabela.\nErro: %s\n", PQerrorMessage(conn));
        close_connection(conn);
    }
}
/*
void show_table(PGconn *conn)
{
    PGresult *resultado;
    char query[LEN];

    fprintf(stdout, "Digite a ")
}
*/
char *getType(char *nomeTab)
{
    char type[LEN];
    char consulta[LEN];
    char variavel[LEN];
    char arrayQuery[40][LEN];
    char nomeVariavel[LEN];
    int op, j, lenQuery = 0;
    fprintf(stdout, "Tipos disponiveis de variaveis: ");
    fprintf(stdout, "1 - Inteiro\n2 - Decimal\n3 - varchar\n: ");

    /*
    while (op != 1 || op != 2 || op != 3)
    {
        fprintf(stdout, "Por favor digite apenas os numeros permitidos.\n");
        fprintf(stdout, "1 - Inteiro\n2 - Decimal\n3 - varchar\n: ");
        scanf("%d", &op);
    }
    */
    for (int i=0 ; i < 40; i++)
    {
        j += 1;
        fprintf(stdout, "Digite o nome da variavel: ");
        gets(nomeVariavel);
        
        fflush(stdin);

        fprintf(stdout, "Digite o tipo da variavel\nOBS: digite o nome do tipo e nao algum dos numeros mostrados acima.\n: ");
        gets(type);

        fflush(stdin);
        sprintf(arrayQuery[i], "%s %s", nomeVariavel, type);

        fprintf(stdout, "Deseja adicionar mais?\n1 - Sim\n2 - Nao\n: ");
        scanf("%d", &op);
        if (op == 2)
            break;
    }

    for (int i=0; i < j; i++)
    {
        strcat(consulta, arrayQuery[i]);
        strcat(consulta, ",");
    }
   
    lenQuery = strlen(consulta);
    consulta[lenQuery - 1] = '\0';

    fprintf("String\n%s\n", consulta); 
    //sprintf(consulta, "CREATE TABLE %s (%s)", nomeTab, nomeVariavel);
    

}