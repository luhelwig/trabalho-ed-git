#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <windows.h>
#include "data.h"

int main() {
    SetConsoleOutputCP(1252);
    SetConsoleCP(1252);

    TabelaEmendas tabela;
    inicializar_tabela(&tabela, 1000);
    NoArvore* indice_autores = NULL;
    
    // Inicialização da nova tabela de Convênios
    TabelaConvenios tab_convenios;
    inicializar_tabela_convenios(&tab_convenios, 1000);

    printf("Carregando banco de dados...\n");
    carregar_dados("EmendasParlamentares.csv", &tabela, &indice_autores);
    carregar_convenios("EmendasParlamentares_Convenios.csv", &tab_convenios);

    int opcao;
    do {
        printf("\n==========================================\n");
        printf(" SISTEMA DE CONSULTA - EMENDAS FEDERAIS\n");
        printf("==========================================\n");
        printf("1. Consultar emendas por Autor\n");
        printf("2. Listar Top 10 Maiores Emendas\n");
        printf("3. Consultar emendas por Area (Estado e Cidade)\n");
        printf("4. Consultar emendas de Convenios (por Convenente)\n"); 
        printf("5. Consultar Convenios por Funcao e Convenente\n"); // <-- ADICIONE NO MENU
        printf("0. Sair\n");
        printf("Escolha uma opcao: ");
        
        if(scanf("%d", &opcao) != 1) { 
            while(getchar() != '\n'); 
            continue; 
        }
        getchar(); 

        switch (opcao) {
            case 1: {
                char letra;
                printf("\nDigite a PRIMEIRA LETRA do nome do Autor: ");
                scanf("%c", &letra);
                getchar(); 
                letra = toupper((unsigned char)letra); 

                printf("\n--- Autores que comecam com a letra '%c' ---\n", letra);
                listar_autores_arvore_por_letra(indice_autores, letra);
                printf("--------------------------------------------\n\n");
                
                char autor_busca[MAX_STR];
                printf("Agora, digite o NOME EXATO do Autor na lista (ou 0 para cancelar): ");
                fgets(autor_busca, MAX_STR, stdin);
                autor_busca[strcspn(autor_busca, "\n")] = 0; 
                
                if (strcmp(autor_busca, "0") != 0) {
                    for(int i = 0; autor_busca[i] != '\0'; i++) {
                        autor_busca[i] = toupper((unsigned char)autor_busca[i]);
                    }
                    buscar_autor_arvore(indice_autores, autor_busca, &tabela);
                }
                break;
            }
            case 2: {
                printf("\nOrdenando dados por Valor Empenhado (Decrescente)...\n");
                quicksort(&tabela, 0, tabela.tamanho - 1);
                
                printf("\n--- TOP 10 MAIORES EMENDAS ---\n");
                for(int i = 0; i < 10 && i < tabela.tamanho; i++) {
                    printf("%d. Autor: %s\n", i+1, tabela.dados[i].autor);
                    printf("   Localidade: %s | Valor: R$ %.2f\n", tabela.dados[i].localidade, tabela.dados[i].valor_empenhado);
                }
                break;
            }
            case 3: {
                char uf_busca[MAX_STR];
                char mun_busca[MAX_STR];

                printf("\nDigite o Estado (ex: MINAS GERAIS ou ACRE): ");
                fgets(uf_busca, MAX_STR, stdin);
                uf_busca[strcspn(uf_busca, "\n")] = 0;
                for(int i = 0; uf_busca[i] != '\0'; i++) uf_busca[i] = toupper((unsigned char)uf_busca[i]);

                printf("Digite o nome da Cidade: ");
                fgets(mun_busca, MAX_STR, stdin);
                mun_busca[strcspn(mun_busca, "\n")] = 0;
                for(int i = 0; mun_busca[i] != '\0'; i++) mun_busca[i] = toupper((unsigned char)mun_busca[i]);

                buscar_por_area(&tabela, uf_busca, mun_busca);
                break;
            }
            case 4: {
                char termo[MAX_STR];
                printf("\nDigite o nome (ou parte do nome) do Convenente (ex: PREFEITURA, HOSPITAL): ");
                fgets(termo, MAX_STR, stdin);
                termo[strcspn(termo, "\n")] = 0;
                
                // Converte a busca para maiúsculo
                for(int i = 0; termo[i] != '\0'; i++) {
                    termo[i] = toupper((unsigned char)termo[i]);
                }
                
                buscar_convenio_por_convenente(&tab_convenios, termo);
                break;
            }
            case 5: {
                char funcao_busca[MAX_STR];
                char conv_busca[MAX_STR];

                // Para a função, respeitamos letras maiúsculas/minúsculas do CSV
                printf("\nDigite a Funcao exata (ex: Saude, Cultura, Educacao): ");
                fgets(funcao_busca, MAX_STR, stdin);
                funcao_busca[strcspn(funcao_busca, "\n")] = 0;

                printf("Digite o nome (ou parte) do Convenente (ex: PREFEITURA): ");
                fgets(conv_busca, MAX_STR, stdin);
                conv_busca[strcspn(conv_busca, "\n")] = 0;
                
                // Transforma convenente em maiúsculo (pois no CSV eles são MAIÚSCULOS)
                for(int i = 0; conv_busca[i] != '\0'; i++) {
                    conv_busca[i] = toupper((unsigned char)conv_busca[i]);
                }

                buscar_convenio_por_funcao_e_convenente(&tab_convenios, funcao_busca, conv_busca);
                break;
            }
            case 0:
                printf("Encerrando programa e limpando memoria...\n");
                break;
            default:
                printf("Opcao invalida!\n");
        }
    } while (opcao != 0);

    // Limpeza das memórias
    liberar_arvore(indice_autores);
    liberar_tabela(&tabela);
    liberar_tabela_convenios(&tab_convenios);
    return 0;
}