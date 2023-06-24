 char valor_varchar[100];
         printf("Digite um valor varchar para a coluna %s: ", nome_coluna);
         scanf("%s", valor_varchar);
         sprintf(consulta, "INSERT INTO %s (%s) VALUES ('%s')", nome_tabela, nome_coluna, valor_varchar);