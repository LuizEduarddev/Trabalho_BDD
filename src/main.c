#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "/usr/include/postgresql/libpq-fe.h"
#include"../files/functions.h"

int main(int argc, char *argv[])
{
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
    char nomeTab[40] = "avioes";

    getType(nomeTab);
    
}