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
void specTable(PGconn *conn);
void tryError(PGconn *conn, PGresult *resultado);
char *cathSwitch(char *nomeCol, char *filter, char *nome_tab);
void showData(PGconn *conn);
void rmData(PGconn *conn);
void rmTabela(PGconn *conn);
void rmItem(PGconn *conn);
void switchItem(PGconn *conn, char *nomeTab, char *filtro, char *nomeCol);

#endif