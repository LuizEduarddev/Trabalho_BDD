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

    while (op != 1)
    {
        char nomeCol[30];
        char tipoCol[30];
        for (int i=0; i < numCol; i++)
        {
            printf("Coluna %d.\n", i);
            printf("Tipo da coluna: %s", pegaTipo(conn, i, tipoCol, resultado));
            printf("Nome da coluna %s", PQfname(resultado, i));
            
            strcpy(nomeCol, PQfname(resultado, i));
            strcpy(tipoCol, pegaTipo(conn, i, tipoCol, resultado));

            funcType(conn, nomeCol, tipoCol, nomeTab);

            fprintf(stdout, "Inserido com sucesso.\n");

            fprintf(stdout, "Deseja inserir mais algum dado na tabela?\n0 - Sim\n1 - Nao");
            scanf("%d", &op);
        }
    }
}

char *pegaTipo(PGconn *conn, int i, char *tipoCol, PGresult *resultado)
{
    Oid columnType = PQftype(resultado, i);
    char *typeName;
    // Consulta o catálogo do sistema pg_type para obter o nome do tipo de dados
    PGresult *typeResult = PQexecParams(conn, "SELECT typname FROM pg_type WHERE oid = $1", 1, NULL, &columnType, NULL, NULL, 0);
    if (PQresultStatus(typeResult) == PGRES_TUPLES_OK && PQntuples(typeResult) > 0) {
        typeName = PQgetvalue(typeResult, 0, 0);
        printf("Tipo de dados da coluna %d: %s\n", i, typeName);
    }

    return typeName;
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

    fprintf(stdout, "String\n%s", consulta); 
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

    char nomeTab[LEN];
    int erro = SUCESSO;
    fprintf(stdout, "Qual tabela deseja ver os campos e tipos de dado?\n: ");
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

    char nomeCol[30];
    char tipoCol[30];
    for (int i=0; i < numCol; i++)
    {
        printf("Coluna %d.\n", i);
        printf("Tipo da coluna: %s\n", pegaTipo(conn, i, tipoCol, resultado));
    }
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
    fprintf(stdout, "Qual tabela deseja ver para filtar?\n: ");
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

    fprintf(stdout, "OK!\nAgora precisamos filtar.\nA filtragem é simples, primeiro escolha uma coluna, a tabela gerada será baseada nesta coluna.\nEX: coluna_nome = joao. O resultado sera uma tabela com todos os nomes joao.");
    
    sprintf(inserir_tabela, "SELECT * FROM %s", nomeTab);
    resultado = PQexec(conn, inserir_tabela);
    tryError(conn, resultado);

    
    fprintf(stdout, "Digite o nome da coluna: ");
    gets(nomeCol);

    fprintf(stdout, "Certo, agora escolha um valor para filtrar\nOBS: O valor pode ser um nome, uma string ou até mesmo um número float ou inteiro\n: ");
    gets(filter);

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

    fprintf(stdout, "Digite o nome da tabela que deseja remover.\n: ");
    gets(nomeTab);

    erro = isinDB(conn, nomeTab);
    while (erro == ERRO)
    {
        fprintf(stdout, "A tabela digitada nao parece estar em nosso banco de dados\nPor favor, tente novamente: ");
        gets(nomeTab);
        erro = isinDB(conn, nomeTab);
    }

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

    fprintf(stdout, "Certo, e de qual tabela deseja apagar algum item?\n: ");
    gets(nomeTab);

    erro = isinDB(conn, nomeTab);
    while (erro == ERRO)
    {
        fprintf(stdout, "A tabela digitada nao parece estar em nosso banco de dados\nPor favor, tente novamente: ");
        gets(nomeTab);
        erro = isinDB(conn, nomeTab);
    }

    sprintf(search, "SELECT * FROM %s", nomeTab);

    resultado = PQexec(conn, search);
    
    fprintf(stdout, "OK!\nAgora digite o valor que desejar deletar\nEX: nome = joao (todos os joao serao apagados da coluna nome.)\n: ");
    gets(filtro);

    fprintf(stdout, "PERFEITO!\nAgora digite o nome da coluna que tera o item deletado.\n: ");
    gets(nomeCol);

    fprintf(stdout, "So mais uma coisinha, digite o tipo da exclusao\n1 - Maior\n2 - Maior ou igual\n3 - Menor\n4 - Menor ou igual\n5 - Diferente de\n6 - Igual");

    switchItem(conn, nomeTab, filtro, nomeCol);
}

void switchItem(PGconn *conn, char *nomeTab, char *filter, char *nomeCol)
{
    int op = 0;
    char retorno[LEN];
    //1 - Maior\n2 - Maior ou igual\n3 - Menor\n4 - Menor ou igual\n5 - Diferente de\n6 - Igual
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