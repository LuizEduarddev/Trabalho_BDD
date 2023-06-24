#ifndef FUNC_H
#define FUNC_H

void close_connection(PGconn *conn);
PGconn* start_connection();
void insert_intodb(PGconn *conn);
void funcType(PGconn *conn, char *nomeCol, char *tipoCol, char *nomeTab);
void GoToInteger(PGconn *conn, char *nomeCol, char *tipoCol, char *nomeTab);
void GoToNumeric(PGconn *conn, char *nomeCol, char *tipoCol, char *nomeTab);
void GoToString(PGconn *conn, char *nomeCol, char *tipoCol, char *nomeTab);
void tabelasDB(PGconn *conn, PGresult *resultado);
int isinDB(PGconn *conn, PGresult *resultado, char *nomeCol);
char *toUper(char *string);
void create_table(PGconn *conn);
char *getType(char *nomeTab);

#endif