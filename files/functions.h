#ifndef FUNC_H
#define FUNC_H

void close_connection(PGconn *conn);
void insert_intodb(PGconn *conn);
void funcType(PGconn *conn, char *nomeCol, char *tipoCol, char *nomeTab);
void GoToInteger(PGconn *conn, char *nomeCol, char *tipoCol, char *nomeTab);
void GoToNumeric(PGconn *conn, char *nomeCol, char *tipoCol, char *nomeTab);
void GoToString(PGconn *conn, char *nomeCol, char *tipoCol, char *nomeTab);
void tabelasDB(PGconn *conn);
int isinDB(PGconn *conn, char *nomeCol);
char *toUper(char *string);
void create_table(PGconn *conn);
char *getType(char *nomeTab);
char *pegaTipo(PGconn *conn, int NumCol, char *tipoCol, PGresult *resultado);

#endif