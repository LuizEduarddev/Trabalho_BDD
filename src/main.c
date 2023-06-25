#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "/usr/include/postgresql/libpq-fe.h"
#include"../files/functions.h"

int main(int argc, char *argv[])
{
    int op = 0;   
    if (argc != 6) {
        printf("Uso: %s <host> <porta> <banco_de_dados> <usuário> <senha>\n", argv[0]);
        return 1;
    }

    char *host = argv[1];
    char *port = argv[2];
    char *database = argv[3];
    char *user = argv[4];
    char *password = argv[5];

    PGconn *conn = PQsetdbLogin(host, port, NULL, NULL, database, user, password);

    // Verifica se a conexão foi estabelecida com sucesso
    if (PQstatus(conn) != CONNECTION_OK) {
        printf("Erro ao conectar ao banco de dados: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        return 1;
    }
    
    fprintf(stdout, "-------------------- MENU DE OPCOES --------------------\n");
    fprintf(stdout, "----- 1 - Lista de tabelas\n");
    fprintf(stdout, "----- 2 - Expecificacoes de campos e tipos de dados\n");
    fprintf(stdout, "----- 3 - Criar tabela\n");
    fprintf(stdout, "----- 4 - Inserir dados em tabela\n");
    fprintf(stdout, "----- 5 -  Exibir os dados em uma tabela\n");
    fprintf(stdout, "----- 6 - Remover dados em uma tabela\n");
    fprintf(stdout, "--------------------------------------------------------\n");
    fprintf(stdout, ": ");
    scanf("%d", &op);

    switch(op)
    {
        case 1:
        tabelasDB(conn);
        break;

        case 2:
        specTable;
        break;

        case 3:
        create_table(conn);
        break;

        case 4:
        insert_intodb(conn);
        break;

        case 5:
        showData(conn);
        break;

        case 6:
        rmData(conn);

    }
    
    
}