#ifndef DATA_H
#define DATA_H

#define MAX_LINHA 4096
#define MAX_STR 512

// ============================================================================
// ESTRUTURAS DE DADOS - EMENDAS GERAIS
// ============================================================================

typedef struct {
    char codigo_emenda[MAX_STR];
    int ano;
    char tipo_emenda[MAX_STR];
    char autor[MAX_STR];
    char localidade[MAX_STR];       
    char funcao[MAX_STR];
    double valor_empenhado;
    double valor_liquidado;
    double valor_pago;
} Emenda;

typedef struct {
    Emenda *dados;
    int tamanho;
    int capacidade;
} TabelaEmendas;

typedef struct NoLista {
    int indice_vetor;
    struct NoLista* prox;
} NoLista;

typedef struct NoArvore {
    char chave[MAX_STR]; 
    NoLista* indices;    
    struct NoArvore* esq;
    struct NoArvore* dir;
} NoArvore;

// ============================================================================
// ESTRUTURAS DE DADOS - CONVENIOS (NOVO)
// ============================================================================

typedef struct {
    char codigo_emenda[MAX_STR];
    char funcao[MAX_STR];
    char localidade[MAX_STR];
    char convenente[MAX_STR];
    char objeto[MAX_STR];
    double valor_convenio;
} Convenio;

typedef struct {
    Convenio *dados;
    int tamanho;
    int capacidade;
} TabelaConvenios;

// ============================================================================
// PROTÓTIPOS DAS FUNÇÕES
// ============================================================================

// Manipulação da Tabela Principal
double converter_moeda_br(const char* str_valor);
void inicializar_tabela(TabelaEmendas *tab, int capacidade_inicial);
void adicionar_emenda(TabelaEmendas *tab, Emenda nova_emenda);
void liberar_tabela(TabelaEmendas *tab);

// Árvore Binária e Buscas
NoLista* criar_no_lista(int indice);
NoArvore* criar_no_arvore(const char* chave, int indice);
NoArvore* inserir_arvore(NoArvore* raiz, const char* chave, int indice_vetor);
void buscar_autor_arvore(NoArvore* raiz, const char* autor_buscado, TabelaEmendas* tab);
void listar_autores_arvore_por_letra(NoArvore* raiz, char letra); 
void liberar_arvore(NoArvore* raiz);

// Ordenação
void trocar_emendas(Emenda* a, Emenda* b);
int particionar(TabelaEmendas* tab, int baixo, int alto);
void quicksort(TabelaEmendas* tab, int baixo, int alto);

// Leitura de Arquivo
char* proximo_campo(char** linha_ptr);
void limpar_aspas(char* str);
void carregar_dados(const char* nome_arquivo, TabelaEmendas* tab, NoArvore** raiz_autores);

// Funções dos Convenios 
void inicializar_tabela_convenios(TabelaConvenios *tab, int capacidade_inicial);
void adicionar_convenio(TabelaConvenios *tab, Convenio c);
void liberar_tabela_convenios(TabelaConvenios *tab);
void carregar_convenios(const char* nome_arquivo, TabelaConvenios* tab);
void buscar_convenio_por_convenente(TabelaConvenios* tab, const char* termo);
void buscar_convenio_por_funcao_e_convenente(TabelaConvenios* tab, const char* funcao, const char* convenente); 

#endif // DATA_H