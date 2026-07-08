#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data.h"

// ============================================================================
// FUNÇÕES UTILITÁRIAS E MANIPULAÇÃO DO VETOR DINÂMICO
// ============================================================================

// Copia str->dest com no maximo (tam_dest-1) caracteres, sempre terminando
// em '\0'. Evita estouro de buffer caso um campo do CSV seja maior que
// o esperado (strcpy puro nao protege contra isso).
void copiar_seguro(char* dest, const char* origem, size_t tam_dest) {
    if (!origem || tam_dest == 0) {
        if (tam_dest > 0) dest[0] = '\0';
        return;
    }
    strncpy(dest, origem, tam_dest - 1);
    dest[tam_dest - 1] = '\0';
}

double converter_moeda_br(const char* str_valor) {
    if (!str_valor || strlen(str_valor) == 0) return 0.0;
    char temp[MAX_STR]; // Aumentado para suportar strings imensas
    int j = 0;
    for (int i = 0; str_valor[i] != '\0' && str_valor[i] != '\n' && str_valor[i] != '\r'; i++) {
        if (str_valor[i] == '.' || str_valor[i] == '"') continue; 
        if (str_valor[i] == ',') temp[j++] = '.';
        else if (j < MAX_STR - 1) temp[j++] = str_valor[i]; // Trava de segurança
    }
    temp[j] = '\0';
    return atof(temp);
}

void inicializar_tabela(TabelaEmendas *tab, int capacidade_inicial) {
    tab->capacidade = capacidade_inicial;
    tab->tamanho = 0;
    tab->dados = (Emenda*) malloc(capacidade_inicial * sizeof(Emenda));
    if (tab->dados == NULL) {
        fprintf(stderr, "Erro fatal: falha ao alocar memoria para a tabela de emendas.\n");
        exit(EXIT_FAILURE);
    }
}

void adicionar_emenda(TabelaEmendas *tab, Emenda nova_emenda) {
    if (tab->tamanho == tab->capacidade) {
        tab->capacidade *= 2;
        Emenda* novo_bloco = (Emenda*) realloc(tab->dados, tab->capacidade * sizeof(Emenda));
        if (novo_bloco == NULL) {
            fprintf(stderr, "Erro fatal: falha ao expandir a tabela de emendas.\n");
            exit(EXIT_FAILURE);
        }
        tab->dados = novo_bloco;
    }
    tab->dados[tab->tamanho] = nova_emenda;
    tab->tamanho++;
}

void liberar_tabela(TabelaEmendas *tab) {
    if (tab->dados != NULL) {
        free(tab->dados);
        tab->dados = NULL;
    }
    tab->tamanho = 0;
    tab->capacidade = 0;
}

// ============================================================================
// ÁRVORE BINÁRIA DE BUSCA (ÍNDICE) E BUSCAS SEQUENCIAIS
// ============================================================================

NoLista* criar_no_lista(int indice) {
    NoLista* novo = (NoLista*) malloc(sizeof(NoLista));
    novo->indice_vetor = indice;
    novo->prox = NULL;
    return novo;
}

NoArvore* criar_no_arvore(const char* chave, int indice) {
    NoArvore* novo = (NoArvore*) malloc(sizeof(NoArvore));
    copiar_seguro(novo->chave, chave, MAX_STR);
    novo->indices = criar_no_lista(indice);
    novo->esq = NULL;
    novo->dir = NULL;
    return novo;
}

NoArvore* inserir_arvore(NoArvore* raiz, const char* chave, int indice_vetor) {
    if (raiz == NULL) return criar_no_arvore(chave, indice_vetor);
    
    int cmp = strcmp(chave, raiz->chave);
    if (cmp == 0) {
        NoLista* atual = raiz->indices;
        while (atual->prox != NULL) atual = atual->prox;
        atual->prox = criar_no_lista(indice_vetor);
    } else if (cmp < 0) {
        raiz->esq = inserir_arvore(raiz->esq, chave, indice_vetor);
    } else {
        raiz->dir = inserir_arvore(raiz->dir, chave, indice_vetor);
    }
    return raiz;
}

void buscar_autor_arvore(NoArvore* raiz, const char* autor_buscado, TabelaEmendas* tab) {
    if (raiz == NULL) {
        printf("Nenhuma emenda encontrada para o autor: %s\n", autor_buscado);
        return;
    }
    
    int cmp = strcmp(autor_buscado, raiz->chave);
    if (cmp == 0) {
        printf("\n--- Emendas encontradas para: %s ---\n", autor_buscado);
        NoLista* atual = raiz->indices;
        double total_empenhado = 0;
        
        while (atual != NULL) {
            Emenda e = tab->dados[atual->indice_vetor];
            printf("Localidade: %s | Valor Empenhado: R$ %.2f\n", e.localidade, e.valor_empenhado);
            total_empenhado += e.valor_empenhado;
            atual = atual->prox;
        }
        printf("-> TOTAL EMPENHADO: R$ %.2f\n", total_empenhado);
    } else if (cmp < 0) {
        buscar_autor_arvore(raiz->esq, autor_buscado, tab);
    } else {
        buscar_autor_arvore(raiz->dir, autor_buscado, tab);
    }
}

void listar_autores_arvore_por_letra(NoArvore* raiz, char letra) {
    if (raiz != NULL) {
        listar_autores_arvore_por_letra(raiz->esq, letra); 
        if (raiz->chave[0] == letra) {
            printf("- %s\n", raiz->chave);    
        }
        listar_autores_arvore_por_letra(raiz->dir, letra); 
    }
}


void liberar_arvore(NoArvore* raiz) {
    if (raiz != NULL) {
        liberar_arvore(raiz->esq);
        liberar_arvore(raiz->dir);
        
        NoLista* atual = raiz->indices;
        while (atual != NULL) {
            NoLista* temp = atual;
            atual = atual->prox;
            free(temp);
        }
        free(raiz);
    }
}

// ============================================================================
// ORDENAÇÃO (QUICKSORT)
// ============================================================================

void trocar_emendas(Emenda* a, Emenda* b) {
    Emenda temp = *a;
    *a = *b;
    *b = temp;
}

int particionar(TabelaEmendas* tab, int baixo, int alto) {
    double pivo = tab->dados[alto].valor_empenhado;
    int i = (baixo - 1);
    
    for (int j = baixo; j <= alto - 1; j++) {
        if (tab->dados[j].valor_empenhado >= pivo) {
            i++;
            trocar_emendas(&tab->dados[i], &tab->dados[j]);
        }
    }
    trocar_emendas(&tab->dados[i + 1], &tab->dados[alto]);
    return (i + 1);
}

void quicksort(TabelaEmendas* tab, int baixo, int alto) {
    if (baixo < alto) {
        int pi = particionar(tab, baixo, alto);
        quicksort(tab, baixo, pi - 1);
        quicksort(tab, pi + 1, alto);
    }
}

// ============================================================================
// LEITURA DO ARQUIVO CSV
// ============================================================================

char* proximo_campo(char** linha_ptr) {
    if (*linha_ptr == NULL || **linha_ptr == '\0') return NULL;
    char* inicio = *linha_ptr;
    char* atual = inicio;
    int dentro_aspas = 0;

    while (*atual != '\0') {
        if (*atual == '"') {
            dentro_aspas = !dentro_aspas; // Liga/desliga a leitura de aspas
        } else if (*atual == ';' && !dentro_aspas) {
            *atual = '\0';
            *linha_ptr = atual + 1;
            return inicio;
        }
        atual++;
    }
    *linha_ptr = NULL;
    return inicio;
}

void limpar_aspas(char* str) {
    int i = 0, j = 0;
    while (str[i] != '\0') {
        if (str[i] != '"') str[j++] = str[i];
        i++;
    }
    str[j] = '\0';
}

void carregar_dados(const char* nome_arquivo, TabelaEmendas* tab, NoArvore** raiz_autores) {
    FILE* arquivo = fopen(nome_arquivo, "r");
    if (!arquivo) {
        printf("Erro ao abrir o arquivo %s!\n", nome_arquivo);
        return;
    }

    char linha[MAX_LINHA];
    if (!fgets(linha, MAX_LINHA, arquivo)) return; 

    int cont = 0;
    while (fgets(linha, MAX_LINHA, arquivo)) {
        char* linha_ptr = linha;
        Emenda e;
        
        // Colunas do CSV (0-indexado):
        // 0 Codigo | 1 Ano | 2 Tipo | 3 CodAutor | 4 Autor | 5 NumEmenda |
        // 6 Localidade | 7 CodMunicIBGE | 8 Municipio | 9 CodUF_IBGE |
        // 10 UF | 11 Regiao | 12 CodFuncao | 13 NomeFuncao | 14 CodSubfuncao |
        // 15 NomeSubfuncao | 16 CodPrograma | 17 NomePrograma | 18 CodAcao |
        // 19 NomeAcao | 20 CodPlanoOrc | 21 NomePlanoOrc |
        // 22 ValorEmpenhado | 23 ValorLiquidado | 24 ValorPago | ...
        char* codigo = proximo_campo(&linha_ptr);        // 0
        char* ano = proximo_campo(&linha_ptr);            // 1
        char* tipo = proximo_campo(&linha_ptr);           // 2
        proximo_campo(&linha_ptr);                        // 3  cod_autor (nao usado)
        char* autor = proximo_campo(&linha_ptr);          // 4
        proximo_campo(&linha_ptr);                        // 5  num_emenda (nao usado)
        char* localidade = proximo_campo(&linha_ptr);     // 6

        // Pula 7..12 (Cod Municipio, Municipio, Cod UF, UF, Regiao, Cod Funcao)
        for (int i = 0; i < 6; i++) proximo_campo(&linha_ptr);

        char* funcao = proximo_campo(&linha_ptr);         // 13 Nome Funcao

        // Pula 14..21 (Subfuncao, Programa, Acao, Plano Orcamentario - cod/nome)
        for (int i = 0; i < 8; i++) proximo_campo(&linha_ptr);

        char* valor_empenhado = proximo_campo(&linha_ptr); // 22
        char* valor_liquidado = proximo_campo(&linha_ptr); // 23
        char* valor_pago = proximo_campo(&linha_ptr);      // 24

        // Zera a struct para nao deixar campos com lixo caso algum
        // campo do CSV esteja ausente (linha incompleta).
        memset(&e, 0, sizeof(Emenda));

        if (codigo) { copiar_seguro(e.codigo_emenda, codigo, MAX_STR); limpar_aspas(e.codigo_emenda); }
        if (ano) e.ano = atoi(ano);
        if (tipo) { copiar_seguro(e.tipo_emenda, tipo, MAX_STR); limpar_aspas(e.tipo_emenda); }
        if (autor) { copiar_seguro(e.autor, autor, MAX_STR); limpar_aspas(e.autor); }
        if (localidade) { copiar_seguro(e.localidade, localidade, MAX_STR); limpar_aspas(e.localidade); }
        if (funcao) { copiar_seguro(e.funcao, funcao, MAX_STR); limpar_aspas(e.funcao); }
        
        e.valor_empenhado = converter_moeda_br(valor_empenhado);
        e.valor_liquidado = converter_moeda_br(valor_liquidado);
        e.valor_pago = converter_moeda_br(valor_pago);

        adicionar_emenda(tab, e);
        *raiz_autores = inserir_arvore(*raiz_autores, e.autor, tab->tamanho - 1);
        
        cont++;
    }
    fclose(arquivo);
    printf("Carregamento concluido! %d registros lidos.\n", cont);
}

// ============================================================================
// FUNÇÕES DOS CONVÊNIOS 
// ============================================================================

void inicializar_tabela_convenios(TabelaConvenios *tab, int capacidade_inicial) {
    tab->capacidade = capacidade_inicial;
    tab->tamanho = 0;
    tab->dados = (Convenio*) malloc(capacidade_inicial * sizeof(Convenio));
    if (tab->dados == NULL) {
        fprintf(stderr, "Erro fatal: falha ao alocar memoria para a tabela de convenios.\n");
        exit(EXIT_FAILURE);
    }
}

void adicionar_convenio(TabelaConvenios *tab, Convenio c) {
    if (tab->tamanho == tab->capacidade) {
        tab->capacidade *= 2;
        Convenio* novo_bloco = (Convenio*) realloc(tab->dados, tab->capacidade * sizeof(Convenio));
        if (novo_bloco == NULL) {
            fprintf(stderr, "Erro fatal: falha ao expandir a tabela de convenios.\n");
            exit(EXIT_FAILURE);
        }
        tab->dados = novo_bloco;
    }
    tab->dados[tab->tamanho] = c;
    tab->tamanho++;
}

void liberar_tabela_convenios(TabelaConvenios *tab) {
    if (tab->dados != NULL) {
        free(tab->dados);
        tab->dados = NULL;
    }
    tab->tamanho = 0;
    tab->capacidade = 0;
}

void carregar_convenios(const char* nome_arquivo, TabelaConvenios* tab) {
    FILE* arquivo = fopen(nome_arquivo, "r");
    if (!arquivo) {
        printf("Erro ao abrir o arquivo %s!\n", nome_arquivo);
        return;
    }

    char linha[MAX_LINHA];
    if (!fgets(linha, MAX_LINHA, arquivo)) return; 

    int cont = 0;
    while (fgets(linha, MAX_LINHA, arquivo)) {
        char* linha_ptr = linha;
        Convenio c;
        
        char* codigo = proximo_campo(&linha_ptr);
        proximo_campo(&linha_ptr); 
        char* funcao = proximo_campo(&linha_ptr);
        proximo_campo(&linha_ptr); 
        proximo_campo(&linha_ptr); 
        char* localidade = proximo_campo(&linha_ptr);
        proximo_campo(&linha_ptr); 
        proximo_campo(&linha_ptr); 
        char* convenente = proximo_campo(&linha_ptr);
        char* objeto = proximo_campo(&linha_ptr);
        proximo_campo(&linha_ptr); 
        char* valor = proximo_campo(&linha_ptr);

        memset(&c, 0, sizeof(Convenio));

        if (codigo) { copiar_seguro(c.codigo_emenda, codigo, MAX_STR); limpar_aspas(c.codigo_emenda); }
        if (funcao) { copiar_seguro(c.funcao, funcao, MAX_STR); limpar_aspas(c.funcao); }
        if (localidade) { copiar_seguro(c.localidade, localidade, MAX_STR); limpar_aspas(c.localidade); }
        if (convenente) { copiar_seguro(c.convenente, convenente, MAX_STR); limpar_aspas(c.convenente); }
        if (objeto) { copiar_seguro(c.objeto, objeto, MAX_STR); limpar_aspas(c.objeto); }
        
        c.valor_convenio = converter_moeda_br(valor);

        adicionar_convenio(tab, c);
        cont++;
    }
    fclose(arquivo);
    printf("Carregamento de convenios concluido! %d registros lidos.\n", cont);
}

void buscar_convenio_por_convenente(TabelaConvenios* tab, const char* termo) {
    double total = 0;
    int cont = 0;
    
    printf("\n--- Convenios encontrados contendo o termo: '%s' ---\n", termo);
    for(int i = 0; i < tab->tamanho; i++) {
        // A função strstr verifica se a palavra digitada existe dentro do nome do Convenente
        if(strstr(tab->dados[i].convenente, termo) != NULL) {
            printf("Convenente: %s\n", tab->dados[i].convenente);
            printf("Objeto: %s\n", tab->dados[i].objeto);
            printf("Localidade: %s | Valor: R$ %.2f\n", tab->dados[i].localidade, tab->dados[i].valor_convenio);
            printf("--------------------------------------------------\n");
            total += tab->dados[i].valor_convenio;
            cont++;
        }
    }
    
    if(cont == 0) {
        printf("Nenhum convenio encontrado com esse termo.\n");
    } else {
        printf("-> %d convenios listados. TOTAL DE RECURSOS: R$ %.2f\n", cont, total);
    }
}

void buscar_convenio_por_funcao_e_convenente(TabelaConvenios* tab, const char* funcao, const char* convenente) {
    double total = 0;
    int cont = 0;
    
    printf("\n--- Convenios encontrados (Funcao: '%s' | Convenente: '%s') ---\n", funcao, convenente);
    for(int i = 0; i < tab->tamanho; i++) {
        // Verifica se o texto digitado existe dentro da funcao E dentro do convenente
        if(strstr(tab->dados[i].funcao, funcao) != NULL && strstr(tab->dados[i].convenente, convenente) != NULL) {
            printf("Funcao: %s\n", tab->dados[i].funcao);
            printf("Convenente: %s\n", tab->dados[i].convenente);
            printf("Objeto: %s\n", tab->dados[i].objeto);
            printf("Localidade: %s | Valor: R$ %.2f\n", tab->dados[i].localidade, tab->dados[i].valor_convenio);
            printf("--------------------------------------------------\n");
            total += tab->dados[i].valor_convenio;
            cont++;
        }
    }
    
    if(cont == 0) {
        printf("Nenhum convenio encontrado com esses criterios.\n");
    } else {
        printf("-> %d convenios listados. TOTAL DE RECURSOS: R$ %.2f\n", cont, total);
    }
}
