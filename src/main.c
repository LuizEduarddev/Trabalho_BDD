#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "/usr/include/postgresql/libpq-fe.h"
#include"../files/functions.h"

int main()
{
    PGconn *conn = start_connection();
    char nomeTab[40] = "avioes";

    getType(nomeTab);
}