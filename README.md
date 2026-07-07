*Breve contexto:
Emendas Parlamentares: São recursos do orçamento público direcionados por deputados e senadores para realizar obras e melhorias em suas bases eleitorais.
Valor Empenhado: É a reserva do dinheiro, o governo garante que aquele recurso está "carimbado" para a obra, mas o valor ainda não saiu dos cofres público.*


*  Visão Geral do Projeto: O projeto foi criado como Trabalho Final da disciplina de Estrutura de Dados para o Instituto de Informática da UFRGS.
A aplicação foi desenvolvida puramente na Linguagem C, o aplicativo é capaz de processar, organizar, indexar e ordenar dados (gigantescos) sobre as Emendas Parlamentares do Executivo Federal,
extraídos do Portal da Transparência do Gov.
O ponto principal da aplicação é sua capacidade de alta performance em filtragens complexas sem a necessidade de nenhum outro banco de dados externo.

Estrutura dos Dados utilizados: 
Vetor Dinâmico Redimensionável (TabelaEmendas)
Índice Remissivo Híbrido (árvore binária de busca + lista encadeada)
Ordenação de Alta Performance (quicksort decrescente)

Recursos:
Linguagem: C
Bibliotecas: #include <stdio.h>, <stdlib.h>, <string.h>, <ctype.h>, <windows.h>, também criamos um "data.h"
