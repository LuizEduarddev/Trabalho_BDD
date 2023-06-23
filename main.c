#include<stdio.h>
#include "/usr/include/postgresql/libpq-fe.h"

int main()
{
    int lib_ver = PQlibVersion();
    printf("Versao libpq: %d\n", lib_ver);
    return 0;
}