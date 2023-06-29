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
void *getType(char *nomeTab, char *result);
char *pegaTipo(char *tipoCol, PGresult *resultado, int numCol);
void specTable(PGconn *conn);
void tryError(PGconn *conn, PGresult *resultado);
void cathSwitch(char *nomeCol, char *filter, char *nomeTab, char *insert);
void showData(PGconn *conn);
void rmData(PGconn *conn);
void rmTabela(PGconn *conn);
void rmItem(PGconn *conn);
void switchItem(PGconn *conn, char *nomeTab, char *filtro, char *nomeCol);
void minuscula(char *str);
void pegatipoFor(int Ncolunas, PGresult *resultado, char *tipoCol, char *nomeCol);
void cathSwitchSTR(char *nomeCol, char *filter, char *nomeTab, char *insert);

#endif