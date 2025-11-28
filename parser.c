#include <stdio.h>
#include <stdlib.h>
#include "tokens.h"

/* ============================
   Interface com o Analisador Lexico (Lexer)
   ============================ */

/* Prototipos do Flex: yylex retorna o tipo do token, yytext o lexema. */
int yylex(void);
extern char *yytext;

#define MAX_TOKENS 4096

// Estrutura para armazenar o tipo e o texto (lexema) de cada token lido.
typedef struct {
    int tipo;
    char lexema[128];
} InformacaoToken;

// Array para guardar todos os tokens lidos da entrada.
InformacaoToken tokens_armazenados[MAX_TOKENS];
// Contador de tokens ja lidos e armazenados.
int quantidade_tokens_lidos = 0;

// Funcao que obtem o proximo token do Lexer (yylex) e o armazena.
int obter_proximo_token() {
    int tk = yylex();
    if (tk == 0) {
        tk = T_EOF; // Trata o fim de arquivo
    }

    if (quantidade_tokens_lidos < MAX_TOKENS) {
        tokens_armazenados[quantidade_tokens_lidos].tipo = tk;

        if (yytext)
            snprintf(tokens_armazenados[quantidade_tokens_lidos].lexema,
                     sizeof(tokens_armazenados[quantidade_tokens_lidos].lexema),
                     "%s", yytext);
        else
            tokens_armazenados[quantidade_tokens_lidos].lexema[0] = '\0';

        quantidade_tokens_lidos++;
    }

    return tk;
}


/* ============================================
   Nao-Terminais (Estruturas Sintaticas)
   ============================================ */

// Enumeracao de todos os simbolos Nao-Terminais da gramatica.
typedef enum {
    NT_PROGRAM = 0,
    NT_MAIN_FUNC,
    NT_LISTA_COMANDOS,
    NT_COMANDO,
    NT_BLOCO,
    NT_DECLARACAO_VAR,
    NT_DECL_VAR_CAUDA,
    NT_ATRIBUICAO,
    NT_COMANDO_LEITURA,
    NT_COMANDO_ESCRITA,
    NT_COMANDO_RETORNO,
    NT_COMANDO_SE,
    NT_ELSE_OPCIONAL,
    NT_COMANDO_ENQUANTO,
    NT_COMANDO_PARA,
    NT_ATRIBUICAO_SIMPLES,
    NT_EXPR_BOOLEANA,
    NT_EXPR_BOOL_RESTO,
    NT_TERMO_BOOL,
    NT_EXPR_RELACIONAL,
    NT_EXPR_REL_RESTO,
    NT_EXPR_ARITMETICA,
    NT_EXPR_ARIT_RESTO,
    NT_TERMO,
    NT_TERMO_RESTO,
    NT_FATOR,
    NUM_NONTERMINALS
} NonTerminal;

/* Simbolos especiais da pilha de analise */
#define EPSILON   (-1) // Simbolo para a producao vazia (epsilon)
#define SIM_FIM   (-2) // Marcador de fundo da pilha/fim da entrada

/* Mapeamento de simbolos: Terminais e Nao-Terminais */
#define SIM_TERMINAL(t)   (t)
#define SIM_NAOTERMINAL(nt)   (NUM_TOKENS + (nt))

#define E_TERMINAL(sim)  ((sim) >= 0 && (sim) < NUM_TOKENS)
#define E_NAOTERMINAL(sim)   ((sim) >= NUM_TOKENS)

/* =======================
   Producoes da Gramatica (Regras de Substituicao)
   ======================= */

#define MAX_RHS 8
#define NUM_PRODUCTIONS 46

// Estrutura para uma regra de producao (Cabeça -> Corpo).
typedef struct {
    int cabeca;              // Nao-Terminal (LHS)
    int corpo[MAX_RHS];     // Sequencia de simbolos (RHS)
    int tam_corpo;          // Tamanho da sequencia RHS
} Producao;

// Array que armazena todas as 46 regras de producao.
Producao producoes[NUM_PRODUCTIONS];

/*
   GRAMATICA (resumida para referencia)
   ...
*/

// Inicializa o array 'producoes' com todas as regras da gramatica.
void inicializar_producoes() {
    int p = 0;

    // 0: PROGRAMA → FUNCAO_MAIN
    producoes[p].cabeca = SIM_NAOTERMINAL(NT_PROGRAM);
    producoes[p].corpo[0] = SIM_NAOTERMINAL(NT_MAIN_FUNC);
    producoes[p].tam_corpo = 1;
    p++;


    // 1: FUNCAO_MAIN → T_TIPO T_MAIN ( ) { LISTA_COMANDOS }
    producoes[p].cabeca   = SIM_NAOTERMINAL(NT_MAIN_FUNC);
    producoes[p].corpo[0] = SIM_TERMINAL(T_TIPO);
    producoes[p].corpo[1] = SIM_TERMINAL(T_MAIN);  // Alterado T_ID
    producoes[p].corpo[2] = SIM_TERMINAL(T_PA);
    producoes[p].corpo[3] = SIM_TERMINAL(T_PF);
    producoes[p].corpo[4] = SIM_TERMINAL(T_CA);
    producoes[p].corpo[5] = SIM_NAOTERMINAL(NT_LISTA_COMANDOS);
    producoes[p].corpo[6] = SIM_TERMINAL(T_CF);
    producoes[p].tam_corpo = 7;
    p++;
     
    
    
    
    // 2: LISTA_COMANDOS → COMANDO LISTA_COMANDOS
    producoes[p].cabeca = SIM_NAOTERMINAL(NT_LISTA_COMANDOS);
    producoes[p].corpo[0] = SIM_NAOTERMINAL(NT_COMANDO);
    producoes[p].corpo[1] = SIM_NAOTERMINAL(NT_LISTA_COMANDOS);
    producoes[p].tam_corpo = 2;
    p++;

    // 3: LISTA_COMANDOS → ε (vazio)
    producoes[p].cabeca = SIM_NAOTERMINAL(NT_LISTA_COMANDOS);
    producoes[p].tam_corpo = 0;
    p++;

    // 4: COMANDO → DECLARACAO_VAR
    producoes[p].cabeca = SIM_NAOTERMINAL(NT_COMANDO);
    producoes[p].corpo[0] = SIM_NAOTERMINAL(NT_DECLARACAO_VAR);
    producoes[p].tam_corpo = 1;
    p++;

    // 5: COMANDO → ATRIBUICAO
    producoes[p].cabeca = SIM_NAOTERMINAL(NT_COMANDO);
    producoes[p].corpo[0] = SIM_NAOTERMINAL(NT_ATRIBUICAO);
    producoes[p].tam_corpo = 1;
    p++;

    // 6: COMANDO → COMANDO_SE
    producoes[p].cabeca = SIM_NAOTERMINAL(NT_COMANDO);
    producoes[p].corpo[0] = SIM_NAOTERMINAL(NT_COMANDO_SE);
    producoes[p].tam_corpo = 1;
    p++;

    // 7: COMANDO → COMANDO_ENQUANTO
    producoes[p].cabeca = SIM_NAOTERMINAL(NT_COMANDO);
    producoes[p].corpo[0] = SIM_NAOTERMINAL(NT_COMANDO_ENQUANTO);
    producoes[p].tam_corpo = 1;
    p++;

    // 8: COMANDO → COMANDO_PARA
    producoes[p].cabeca = SIM_NAOTERMINAL(NT_COMANDO);
    producoes[p].corpo[0] = SIM_NAOTERMINAL(NT_COMANDO_PARA);
    producoes[p].tam_corpo = 1;
    p++;

    // 9: COMANDO → COMANDO_LEITURA
    producoes[p].cabeca = SIM_NAOTERMINAL(NT_COMANDO);
    producoes[p].corpo[0] = SIM_NAOTERMINAL(NT_COMANDO_LEITURA);
    producoes[p].tam_corpo = 1;
    p++;

    // 10: COMANDO → COMANDO_ESCRITA
    producoes[p].cabeca = SIM_NAOTERMINAL(NT_COMANDO);
    producoes[p].corpo[0] = SIM_NAOTERMINAL(NT_COMANDO_ESCRITA);
    producoes[p].tam_corpo = 1;
    p++;

    // 11: COMANDO → COMANDO_RETORNO
    producoes[p].cabeca = SIM_NAOTERMINAL(NT_COMANDO);
    producoes[p].corpo[0] = SIM_NAOTERMINAL(NT_COMANDO_RETORNO);
    producoes[p].tam_corpo = 1;
    p++;

    // 12: COMANDO → BLOCO
    producoes[p].cabeca = SIM_NAOTERMINAL(NT_COMANDO);
    producoes[p].corpo[0] = SIM_NAOTERMINAL(NT_BLOCO);
    producoes[p].tam_corpo = 1;
    p++;

    // 13: BLOCO → { LISTA_COMANDOS }
    producoes[p].cabeca = SIM_NAOTERMINAL(NT_BLOCO);
    producoes[p].corpo[0] = SIM_TERMINAL(T_CA);
    producoes[p].corpo[1] = SIM_NAOTERMINAL(NT_LISTA_COMANDOS);
    producoes[p].corpo[2] = SIM_TERMINAL(T_CF);
    producoes[p].tam_corpo = 3;
    p++;

    // 14: DECLARACAO_VAR → T_TIPO T_ID DECL_VAR_CAUDA
    producoes[p].cabeca = SIM_NAOTERMINAL(NT_DECLARACAO_VAR);
    producoes[p].corpo[0] = SIM_TERMINAL(T_TIPO);
    producoes[p].corpo[1] = SIM_TERMINAL(T_ID);
    producoes[p].corpo[2] = SIM_NAOTERMINAL(NT_DECL_VAR_CAUDA);
    producoes[p].tam_corpo = 3;
    p++;

    // 15: DECL_VAR_CAUDA → = EXPR_ARITMETICA ;
    producoes[p].cabeca = SIM_NAOTERMINAL(NT_DECL_VAR_CAUDA);
    producoes[p].corpo[0] = SIM_TERMINAL(T_IGUAL);
    producoes[p].corpo[1] = SIM_NAOTERMINAL(NT_EXPR_ARITMETICA);
    producoes[p].corpo[2] = SIM_TERMINAL(T_PV);
    producoes[p].tam_corpo = 3;
    p++;

    // 16: DECL_VAR_CAUDA → ;
    producoes[p].cabeca = SIM_NAOTERMINAL(NT_DECL_VAR_CAUDA);
    producoes[p].corpo[0] = SIM_TERMINAL(T_PV);
    producoes[p].tam_corpo = 1;
    p++;

    // 17: ATRIBUICAO → T_ID = EXPR_ARITMETICA ;
    producoes[p].cabeca = SIM_NAOTERMINAL(NT_ATRIBUICAO);
    producoes[p].corpo[0] = SIM_TERMINAL(T_ID);
    producoes[p].corpo[1] = SIM_TERMINAL(T_IGUAL);
    producoes[p].corpo[2] = SIM_NAOTERMINAL(NT_EXPR_ARITMETICA);
    producoes[p].corpo[3] = SIM_TERMINAL(T_PV);
    producoes[p].tam_corpo = 4;
    p++;

    // 18: COMANDO_LEITURA → read ID ;
    producoes[p].cabeca = SIM_NAOTERMINAL(NT_COMANDO_LEITURA);
    producoes[p].corpo[0] = SIM_TERMINAL(T_READ);
    producoes[p].corpo[1] = SIM_TERMINAL(T_ID);
    producoes[p].corpo[2] = SIM_TERMINAL(T_PV);
    producoes[p].tam_corpo = 3;
    p++;

    // 19: COMANDO_ESCRITA → print EXPR_ARITMETICA ;
    producoes[p].cabeca = SIM_NAOTERMINAL(NT_COMANDO_ESCRITA);
    producoes[p].corpo[0] = SIM_TERMINAL(T_PRINT);
    producoes[p].corpo[1] = SIM_NAOTERMINAL(NT_EXPR_ARITMETICA);
    producoes[p].corpo[2] = SIM_TERMINAL(T_PV);
    producoes[p].tam_corpo = 3;
    p++;

    // 20: COMANDO_RETORNO → return EXPR_ARITMETICA ;
    producoes[p].cabeca = SIM_NAOTERMINAL(NT_COMANDO_RETORNO);
    producoes[p].corpo[0] = SIM_TERMINAL(T_RETURN);
    producoes[p].corpo[1] = SIM_NAOTERMINAL(NT_EXPR_ARITMETICA);
    producoes[p].corpo[2] = SIM_TERMINAL(T_PV);
    producoes[p].tam_corpo = 3;
    p++;

    // 21: COMANDO_SE → if ( EXPR_BOOLEANA ) BLOCO ELSE_OPCIONAL
    producoes[p].cabeca = SIM_NAOTERMINAL(NT_COMANDO_SE);
    producoes[p].corpo[0] = SIM_TERMINAL(T_IF);
    producoes[p].corpo[1] = SIM_TERMINAL(T_PA);
    producoes[p].corpo[2] = SIM_NAOTERMINAL(NT_EXPR_BOOLEANA);
    producoes[p].corpo[3] = SIM_TERMINAL(T_PF);
    producoes[p].corpo[4] = SIM_NAOTERMINAL(NT_BLOCO);
    producoes[p].corpo[5] = SIM_NAOTERMINAL(NT_ELSE_OPCIONAL);
    producoes[p].tam_corpo = 6;
    p++;

    // 22: ELSE_OPCIONAL → else BLOCO
    producoes[p].cabeca = SIM_NAOTERMINAL(NT_ELSE_OPCIONAL);
    producoes[p].corpo[0] = SIM_TERMINAL(T_ELSE);
    producoes[p].corpo[1] = SIM_NAOTERMINAL(NT_BLOCO);
    producoes[p].tam_corpo = 2;
    p++;

    // 23: ELSE_OPCIONAL → ε
    producoes[p].cabeca = SIM_NAOTERMINAL(NT_ELSE_OPCIONAL);
    producoes[p].tam_corpo = 0;
    p++;

    // 24: COMANDO_ENQUANTO → while ( EXPR_BOOLEANA ) BLOCO
    producoes[p].cabeca = SIM_NAOTERMINAL(NT_COMANDO_ENQUANTO);
    producoes[p].corpo[0] = SIM_TERMINAL(T_WHILE);
    producoes[p].corpo[1] = SIM_TERMINAL(T_PA);
    producoes[p].corpo[2] = SIM_NAOTERMINAL(NT_EXPR_BOOLEANA);
    producoes[p].corpo[3] = SIM_TERMINAL(T_PF);
    producoes[p].corpo[4] = SIM_NAOTERMINAL(NT_BLOCO);
    producoes[p].tam_corpo = 5;
    p++;

    // 25: COMANDO_PARA → for ( ATRIBUICAO_SIMPLES ; EXPR_BOOLEANA ; ATRIBUICAO_SIMPLES )
    producoes[p].cabeca = SIM_NAOTERMINAL(NT_COMANDO_PARA);
    producoes[p].corpo[0] = SIM_TERMINAL(T_FOR);
    producoes[p].corpo[1] = SIM_TERMINAL(T_PA);
    producoes[p].corpo[2] = SIM_NAOTERMINAL(NT_ATRIBUICAO_SIMPLES);
    producoes[p].corpo[3] = SIM_TERMINAL(T_PV);
    producoes[p].corpo[4] = SIM_NAOTERMINAL(NT_EXPR_BOOLEANA);
    producoes[p].corpo[5] = SIM_TERMINAL(T_PV);
    producoes[p].corpo[6] = SIM_NAOTERMINAL(NT_ATRIBUICAO_SIMPLES);
    producoes[p].corpo[7] = SIM_TERMINAL(T_PF);
    producoes[p].tam_corpo = 8;
    p++;

    // 26: ATRIBUICAO_SIMPLES → ID = EXPR_ARITMETICA
    producoes[p].cabeca = SIM_NAOTERMINAL(NT_ATRIBUICAO_SIMPLES);
    producoes[p].corpo[0] = SIM_TERMINAL(T_ID);
    producoes[p].corpo[1] = SIM_TERMINAL(T_IGUAL);
    producoes[p].corpo[2] = SIM_NAOTERMINAL(NT_EXPR_ARITMETICA);
    producoes[p].tam_corpo = 3;
    p++;

    // 27: EXPR_BOOLEANA → TERMO_BOOL EXPR_BOOL_RESTO
    producoes[p].cabeca = SIM_NAOTERMINAL(NT_EXPR_BOOLEANA);
    producoes[p].corpo[0] = SIM_NAOTERMINAL(NT_TERMO_BOOL);
    producoes[p].corpo[1] = SIM_NAOTERMINAL(NT_EXPR_BOOL_RESTO);
    producoes[p].tam_corpo = 2;
    p++;

    // 28: EXPR_BOOL_RESTO → OP_LOG TERMO_BOOL EXPR_BOOL_RESTO
    producoes[p].cabeca = SIM_NAOTERMINAL(NT_EXPR_BOOL_RESTO);
    producoes[p].corpo[0] = SIM_TERMINAL(T_OP_LOG);
    producoes[p].corpo[1] = SIM_NAOTERMINAL(NT_TERMO_BOOL);
    producoes[p].corpo[2] = SIM_NAOTERMINAL(NT_EXPR_BOOL_RESTO);
    producoes[p].tam_corpo = 3;
    p++;

    // 29: EXPR_BOOL_RESTO → ε
    producoes[p].cabeca = SIM_NAOTERMINAL(NT_EXPR_BOOL_RESTO);
    producoes[p].tam_corpo = 0;
    p++;

    // 30: TERMO_BOOL → ! TERMO_BOOL
    producoes[p].cabeca = SIM_NAOTERMINAL(NT_TERMO_BOOL);
    producoes[p].corpo[0] = SIM_TERMINAL(T_NOT);
    producoes[p].corpo[1] = SIM_NAOTERMINAL(NT_TERMO_BOOL);
    producoes[p].tam_corpo = 2;
    p++;

    // 31: TERMO_BOOL → EXPR_RELACIONAL
    producoes[p].cabeca = SIM_NAOTERMINAL(NT_TERMO_BOOL);
    producoes[p].corpo[0] = SIM_NAOTERMINAL(NT_EXPR_RELACIONAL);
    producoes[p].tam_corpo = 1;
    p++;

    // 32: EXPR_RELACIONAL → EXPR_ARITMETICA EXPR_REL_RESTO
    producoes[p].cabeca = SIM_NAOTERMINAL(NT_EXPR_RELACIONAL);
    producoes[p].corpo[0] = SIM_NAOTERMINAL(NT_EXPR_ARITMETICA);
    producoes[p].corpo[1] = SIM_NAOTERMINAL(NT_EXPR_REL_RESTO);
    producoes[p].tam_corpo = 2;
    p++;

    // 33: EXPR_REL_RESTO → OP_COM EXPR_ARITMETICA
    producoes[p].cabeca = SIM_NAOTERMINAL(NT_EXPR_REL_RESTO);
    producoes[p].corpo[0] = SIM_TERMINAL(T_OP_COM);
    producoes[p].corpo[1] = SIM_NAOTERMINAL(NT_EXPR_ARITMETICA);
    producoes[p].tam_corpo = 2;
    p++;

    // 34: EXPR_REL_RESTO → ε
    producoes[p].cabeca = SIM_NAOTERMINAL(NT_EXPR_REL_RESTO);
    producoes[p].tam_corpo = 0;
    p++;

    // 35: EXPR_ARITMETICA → TERMO EXPR_ARIT_RESTO
    producoes[p].cabeca = SIM_NAOTERMINAL(NT_EXPR_ARITMETICA);
    producoes[p].corpo[0] = SIM_NAOTERMINAL(NT_TERMO);
    producoes[p].corpo[1] = SIM_NAOTERMINAL(NT_EXPR_ARIT_RESTO);
    producoes[p].tam_corpo = 2;
    p++;

    // 36: EXPR_ARIT_RESTO → + TERMO EXPR_ARIT_RESTO
    producoes[p].cabeca = SIM_NAOTERMINAL(NT_EXPR_ARIT_RESTO);
    producoes[p].corpo[0] = SIM_TERMINAL(T_SOMA);
    producoes[p].corpo[1] = SIM_NAOTERMINAL(NT_TERMO);
    producoes[p].corpo[2] = SIM_NAOTERMINAL(NT_EXPR_ARIT_RESTO);
    producoes[p].tam_corpo = 3;
    p++;

    // 37: EXPR_ARIT_RESTO → - TERMO EXPR_ARIT_RESTO
    producoes[p].cabeca = SIM_NAOTERMINAL(NT_EXPR_ARIT_RESTO);
    producoes[p].corpo[0] = SIM_TERMINAL(T_SUB);
    producoes[p].corpo[1] = SIM_NAOTERMINAL(NT_TERMO);
    producoes[p].corpo[2] = SIM_NAOTERMINAL(NT_EXPR_ARIT_RESTO);
    producoes[p].tam_corpo = 3;
    p++;

    // 38: EXPR_ARIT_RESTO → ε
    producoes[p].cabeca = SIM_NAOTERMINAL(NT_EXPR_ARIT_RESTO);
    producoes[p].tam_corpo = 0;
    p++;

    // 39: TERMO → FATOR TERMO_RESTO
    producoes[p].cabeca = SIM_NAOTERMINAL(NT_TERMO);
    producoes[p].corpo[0] = SIM_NAOTERMINAL(NT_FATOR);
    producoes[p].corpo[1] = SIM_NAOTERMINAL(NT_TERMO_RESTO);
    producoes[p].tam_corpo = 2;
    p++;

    // 40: TERMO_RESTO → * FATOR TERMO_RESTO
    producoes[p].cabeca = SIM_NAOTERMINAL(NT_TERMO_RESTO);
    producoes[p].corpo[0] = SIM_TERMINAL(T_MUL);
    producoes[p].corpo[1] = SIM_NAOTERMINAL(NT_FATOR);
    producoes[p].corpo[2] = SIM_NAOTERMINAL(NT_TERMO_RESTO);
    producoes[p].tam_corpo = 3;
    p++;

    // 41: TERMO_RESTO → / FATOR TERMO_RESTO
    producoes[p].cabeca = SIM_NAOTERMINAL(NT_TERMO_RESTO);
    producoes[p].corpo[0] = SIM_TERMINAL(T_DIV);
    producoes[p].corpo[1] = SIM_NAOTERMINAL(NT_FATOR);
    producoes[p].corpo[2] = SIM_NAOTERMINAL(NT_TERMO_RESTO);
    producoes[p].tam_corpo = 3;
    p++;

    // 42: TERMO_RESTO → ε
    producoes[p].cabeca = SIM_NAOTERMINAL(NT_TERMO_RESTO);
    producoes[p].tam_corpo = 0;
    p++;

    // 43: FATOR → ( EXPR_BOOLEANA )
    producoes[p].cabeca = SIM_NAOTERMINAL(NT_FATOR);
    producoes[p].corpo[0] = SIM_TERMINAL(T_PA);
    producoes[p].corpo[1] = SIM_NAOTERMINAL(NT_EXPR_BOOLEANA);
    producoes[p].corpo[2] = SIM_TERMINAL(T_PF);
    producoes[p].tam_corpo = 3;
    p++;

    // 44: FATOR → ID
    producoes[p].cabeca = SIM_NAOTERMINAL(NT_FATOR);
    producoes[p].corpo[0] = SIM_TERMINAL(T_ID);
    producoes[p].tam_corpo = 1;
    p++;

    // 45: FATOR → NUM
    producoes[p].cabeca = SIM_NAOTERMINAL(NT_FATOR);
    producoes[p].corpo[0] = SIM_TERMINAL(T_NUM);
    producoes[p].tam_corpo = 1;
    p++;
}

/* ===========================
   Calculo dos Conjuntos e Tabela LL(1)
   =========================== */

// Tabela LL(1): M[Nao-Terminal, Terminal] = Indice da Producao.
int tabela_analise[NUM_NONTERMINALS][NUM_TOKENS];
// Conjunto de terminais que podem iniciar uma derivacao de um NT.
static int conjunto_first[NUM_NONTERMINALS][NUM_TOKENS];
// Conjunto de terminais que podem seguir um NT na cadeia.
static int conjunto_follow[NUM_NONTERMINALS][NUM_TOKENS];
// Indica se um Nao-Terminal pode derivar a string vazia (e anulavel).
static int anulavel[NUM_NONTERMINALS];

// Calcula quais nao-terminais podem derivar a string vazia (ε).
void calcular_anulaveis() {
    for (int i = 0; i < NUM_NONTERMINALS; i++)
        anulavel[i] = 0;

    int changed = 1;
    while (changed) {
        changed = 0;
        for (int p = 0; p < NUM_PRODUCTIONS; p++) {
            Producao *prod = &producoes[p];
            int A = prod->cabeca - NUM_TOKENS;

            if (anulavel[A])
                continue;

            if (prod->tam_corpo == 0) {
                anulavel[A] = 1;
                changed = 1;
            } else {
                int allNullable = 1;
                for (int i = 0; i < prod->tam_corpo; i++) {
                    int sim = prod->corpo[i];
                    if (E_TERMINAL(sim)) {
                        allNullable = 0;
                        break;
                    } else {
                        int B = sim - NUM_TOKENS;
                        if (!anulavel[B]) {
                            allNullable = 0;
                            break;
                        }
                    }
                }
                if (allNullable) {
                    anulavel[A] = 1;
                    changed = 1;
                }
            }
        }
    }
}

// Calcula o conjunto FIRST para todos os Nao-Terminais.
void calcular_conjuntos_first() {
    for (int i = 0; i < NUM_NONTERMINALS; i++)
        for (int t = 0; t < NUM_TOKENS; t++)
            conjunto_first[i][t] = 0;

    int changed = 1;
    while (changed) {
        changed = 0;
        for (int p = 0; p < NUM_PRODUCTIONS; p++) {
            Producao *prod = &producoes[p];
            int A = prod->cabeca - NUM_TOKENS;

            if (prod->tam_corpo == 0) {
                continue;
            }

            for (int i = 0; i < prod->tam_corpo; i++) {
                int sim = prod->corpo[i];
                if (E_TERMINAL(sim)) {
                    if (!conjunto_first[A][sim]) {
                        conjunto_first[A][sim] = 1;
                        changed = 1;
                    }
                    break;
                } else {
                    int B = sim - NUM_TOKENS;
                    for (int t = 0; t < NUM_TOKENS; t++) {
                        if (conjunto_first[B][t] && !conjunto_first[A][t]) {
                            conjunto_first[A][t] = 1;
                            changed = 1;
                        }
                    }
                    if (!anulavel[B])
                        break;
                }
            }
        }
    }
}

// Funcao auxiliar que calcula o FIRST de uma sequencia de simbolos.
void first_de_sequencia(Producao *prod, int pos, int result[NUM_TOKENS], int *seqAnulavel) {
    for (int t = 0; t < NUM_TOKENS; t++)
        result[t] = 0;
    *seqAnulavel = 1;

    if (pos >= prod->tam_corpo) {
        return;
    }

    for (int i = pos; i < prod->tam_corpo; i++) {
        int sim = prod->corpo[i];
        if (E_TERMINAL(sim)) {
            result[sim] = 1;
            *seqAnulavel = 0;
            return;
        } else {
            int B = sim - NUM_TOKENS;
            for (int t = 0; t < NUM_TOKENS; t++) {
                if (conjunto_first[B][t])
                    result[t] = 1;
            }
            if (!anulavel[B]) {
                *seqAnulavel = 0;
                return;
            }
        }
    }
    *seqAnulavel = 1;
}

// Calcula o conjunto FOLLOW para todos os Nao-Terminais.
void calcular_conjuntos_follow() {
    for (int i = 0; i < NUM_NONTERMINALS; i++)
        for (int t = 0; t < NUM_TOKENS; t++)
            conjunto_follow[i][t] = 0;

    // O simbolo de Fim de Arquivo (EOF) 
    conjunto_follow[NT_PROGRAM][T_EOF] = 1;

    int changed = 1;
    while (changed) {
        changed = 0;

        for (int p = 0; p < NUM_PRODUCTIONS; p++) {
            Producao *prod = &producoes[p];
            int A = prod->cabeca - NUM_TOKENS;

            for (int i = 0; i < prod->tam_corpo; i++) {
                int sim = prod->corpo[i];
                if (E_NAOTERMINAL(sim)) {
                    int B = sim - NUM_TOKENS;

                    int firstBeta[NUM_TOKENS];
                    int betaAnulavel;
                    first_de_sequencia(prod, i + 1, firstBeta, &betaAnulavel);

                    // Regra de FOLLOW: FIRST(simbolo seguinte) e adicionado ao FOLLOW(B).
                    for (int t = 0; t < NUM_TOKENS; t++) {
                        if (firstBeta[t]) {
                            if (!conjunto_follow[B][t]) {
                                conjunto_follow[B][t] = 1;
                                changed = 1;
                            }
                        }
                    }

                    // Regra de FOLLOW: Se o resto e anulavel (ε), FOLLOW(A) e adicionado ao FOLLOW(B).
                    if (betaAnulavel || i == prod->tam_corpo - 1) {
                        for (int t = 0; t < NUM_TOKENS; t++) {
                            if (conjunto_follow[A][t] && !conjunto_follow[B][t]) {
                                conjunto_follow[B][t] = 1;
                                changed = 1;
                            }
                        }
                    }
                }
            }
        }
    }
}

// Constroi a tabela LL(1) usando os conjuntos FIRST e FOLLOW calculados.
void construir_tabela_analise_ll1() {
    for (int nt = 0; nt < NUM_NONTERMINALS; nt++)
        for (int t = 0; t < NUM_TOKENS; t++)
            tabela_analise[nt][t] = -1; // Inicializa com erro/vazio

    for (int p = 0; p < NUM_PRODUCTIONS; p++) {
        Producao *prod = &producoes[p];
        int A = prod->cabeca - NUM_TOKENS;

        int firstAlpha[NUM_TOKENS];
        int alphaAnulavel;
        first_de_sequencia(prod, 0, firstAlpha, &alphaAnulavel);

        // Se o lookahead 't' esta no FIRST(alfa), a producao p e usada.
        for (int t = 0; t < NUM_TOKENS; t++) {
            if (firstAlpha[t]) {
                tabela_analise[A][t] = p;
            }
        }

        // Se a producao e anulavel (deriva ε), ela e usada quando o lookahead esta no FOLLOW(A).
        if (alphaAnulavel || prod->tam_corpo == 0) {
            for (int t = 0; t < NUM_TOKENS; t++) {
                if (conjunto_follow[A][t]) {
                    tabela_analise[A][t] = p;
                }
            }
        }
    }
}

/* ==================
   Pilha de analise
   ================== */

// Estrutura de Pilha usada para simular a derivacao.
typedef struct {
    int data[2048];
    int topo; // Indice do topo da pilha.
} Pilha;

void pilha_init(Pilha *s) { s->topo = -1; }
void pilha_push(Pilha *s, int v) { s->data[++(s->topo)] = v; }
int pilha_pop(Pilha *s) {
    if (s->topo < 0) return SIM_FIM;
    return s->data[(s->topo)--];
}
int pilha_peek(Pilha *s) {
    if (s->topo < 0) return SIM_FIM;
    return s->data[s->topo];
}

/* ==============
   Funcao de Analise Sintatica (Parsing)
   ============== */

// Executa a analise sintatica LL(1) da entrada.
int analisar() {
    Pilha pilha;
    pilha_init(&pilha);

    // Empilha marcador de fim e o simbolo inicial da gramatica.
    pilha_push(&pilha, SIM_FIM);
    pilha_push(&pilha, SIM_NAOTERMINAL(NT_PROGRAM));

    // O token atual de entrada (lookahead).
    int token_de_entrada = obter_proximo_token();

    while (1) {
        int topo = pilha_peek(&pilha);

        if (topo == SIM_FIM) {
            // Se o topo da pilha e a entrada acabaram, a analise e um sucesso.
            if (token_de_entrada == T_EOF) {
                printf("\nSucesso: programa sintaticamente correto.\n\n");
                return 1;
            } else {
                printf("\nErro sintatico: tokens restantes na entrada.\n\n");
                return 0;
            }
        }

        if (E_TERMINAL(topo)) {
            // Se for Terminal, tenta dar 'match' com o lookahead.
            if (topo == token_de_entrada) {
               pilha_pop(&pilha); // Consome o simbolo da pilha.
               token_de_entrada = obter_proximo_token(); // Avanca na entrada.
            } else {
                printf("\nErro sintatico: esperado token %d, encontrado %d (%s)\n\n",
                       topo, token_de_entrada, yytext);
                return 0;
            }
        } else {
            // Se for Nao-Terminal, consulta a tabela LL(1).
            int nt = topo - NUM_TOKENS;
            int prod_index = -1;

            if (token_de_entrada >= 0 && token_de_entrada < NUM_TOKENS)
                prod_index = tabela_analise[nt][token_de_entrada];

            if (prod_index < 0) {
                printf("\nErro sintatico: producao inexistente para NT=%d com lookahead=%d (%s)\n\n",
                       nt, token_de_entrada, yytext);
                return 0;
            }

            pilha_pop(&pilha); // Remove o NT.
            Producao *prod = &producoes[prod_index];

            // Empilha o corpo da producao em ordem reversa.
            for (int i = prod->tam_corpo - 1; i >= 0; i--) {
                pilha_push(&pilha, prod->corpo[i]);
            }
        }
    }

    return 0;
}

/* ==================
   Funcoes de Visualizacao e Debug
   ================== */

// Retorna o nome em string de um tipo de token.
const char* token_name(int token) {
    switch(token) {
        case T_MAIN: return "T_MAIN";
        case T_TIPO: return "T_TIPO";
        case T_IF: return "T_IF";
        case T_ELSE: return "T_ELSE";
        case T_WHILE: return "T_WHILE";
        case T_DO: return "T_DO";
        case T_FOR: return "T_FOR";
        case T_RETURN: return "T_RETURN";
        case T_READ: return "T_READ";
        case T_PRINT: return "T_PRINT";
        case T_PV: return "T_PV";
        case T_VIRG: return "T_VIRG";
        case T_IGUAL: return "T_IGUAL";
        case T_PA: return "T_PA";
        case T_PF: return "T_PF";
        case T_CA: return "T_CA";
        case T_CF: return "T_CF";
        case T_SOMA: return "T_SOMA";
        case T_SUB: return "T_SUB";
        case T_MUL: return "T_MUL";
        case T_DIV: return "T_DIV";
        case T_OP_COM: return "T_OP_COM";
        case T_OP_LOG: return "T_OP_LOG";
        case T_NOT: return "T_NOT";
        case T_ID: return "T_ID";
        case T_NUM: return "T_NUM";
        case T_EOF: return "T_EOF";
        case T_ERROR: return "T_ERROR";
        default: return "TOKEN_DESCONHECIDO";
    }
}

// Retorna o nome em string de um Nao-Terminal.
const char* nonterm_name(int nt) {
    switch (nt) {
        case NT_PROGRAM:         return "PROGRAMA";
        case NT_MAIN_FUNC:       return "FUNCAO_MAIN";
    //  case NT_MAIN_BLOCK:      return "BLOCO_MAIN";  nao e usado em producoes 
        case NT_LISTA_COMANDOS:  return "LISTA_COMANDOS";
        case NT_COMANDO:         return "COMANDO";
        case NT_BLOCO:           return "BLOCO";
        case NT_DECLARACAO_VAR:  return "DECLARACAO_VAR";
        case NT_DECL_VAR_CAUDA:  return "DECL_VAR_CAUDA";
        case NT_ATRIBUICAO:      return "ATRIBUICAO";
        case NT_COMANDO_LEITURA: return "COMANDO_LEITURA";
        case NT_COMANDO_ESCRITA: return "COMANDO_ESCRITA";
        case NT_COMANDO_RETORNO: return "COMANDO_RETORNO";
        case NT_COMANDO_SE:      return "COMANDO_SE";
        case NT_ELSE_OPCIONAL:   return "ELSE_OPCIONAL";
        case NT_COMANDO_ENQUANTO:return "COMANDO_ENQUANTO";
        case NT_COMANDO_PARA:    return "COMANDO_PARA";
        case NT_ATRIBUICAO_SIMPLES: return "ATRIBUICAO_SIMPLES";
        case NT_EXPR_BOOLEANA:   return "EXPR_BOOLEANA";
        case NT_EXPR_BOOL_RESTO: return "EXPR_BOOL_RESTO";
        case NT_TERMO_BOOL:      return "TERMO_BOOL";
        case NT_EXPR_RELACIONAL: return "EXPR_RELACIONAL";
        case NT_EXPR_REL_RESTO:  return "EXPR_REL_RESTO";
        case NT_EXPR_ARITMETICA: return "EXPR_ARITMETICA";
        case NT_EXPR_ARIT_RESTO: return "EXPR_ARIT_RESTO";
        case NT_TERMO:           return "TERMO";
        case NT_TERMO_RESTO:     return "TERMO_RESTO";
        case NT_FATOR:           return "FATOR";
        default:                 return "NT?";
    }
}

void imprimir_conjuntos_first() {
    printf("=============== FIRST ======================\n");
    for (int nt = 0; nt < NUM_NONTERMINALS; nt++) {

        // opcional: se não quiser mostrar o NT_MAIN_BLOCK, que não tem produções

        printf("FIRST(%s) = { ", nonterm_name(nt));
        int primeiro = 1;

        for (int t = 0; t < NUM_TOKENS; t++) {
            if (conjunto_first[nt][t]) {
                if (!primeiro) printf(", ");
                printf("%s", token_name(t));
                primeiro = 0;
            }
        }

        // se é anulável, mostra ε
        if (anulavel[nt]) {
            if (!primeiro) printf(", ");
            printf("ε");
        }

        printf(" }\n");
    }
    printf("===========================================\n\n");
}


// Imprime o conjunto FOLLOW de cada Nao-Terminal.
void imprimir_conjuntos_follow() {
    printf("============================== FOLLOW ======================================\n");
    for (int nt = 0; nt < NUM_NONTERMINALS; nt++) {
        printf("FOLLOW(%s) = { ", nonterm_name(nt));
        int firstPrinted = 0;
        for (int t = 0; t < NUM_TOKENS; t++) {
            if (conjunto_follow[nt][t]) {
                if (firstPrinted) printf(", ");
                printf("%s", token_name(t));
                firstPrinted = 1;
            }
        }
        printf(" }\n");
    }
    printf("============================================================================\n\n");
}

// Imprime o conteudo de uma producao (regra da gramatica).
void imprimir_producao(int p) {
    Producao *prod = &producoes[p];
    int A = prod->cabeca - NUM_TOKENS;

    printf("%s -> ", nonterm_name(A));

    if (prod->tam_corpo == 0) {
        printf("e");
    } else {
        for (int i = 0; i < prod->tam_corpo; i++) {
            int sim = prod->corpo[i];
            if (E_TERMINAL(sim)) {
                printf("%s ", token_name(sim));
            } else {
                int B = sim - NUM_TOKENS;
                printf("%s ", nonterm_name(B));
            }
        }
    }
}


// Imprime a tabela de analise sintatica LL(1).
void imprimir_tabela_analise() {
    printf("================================ TABELA LL(1) ================================\n");

    for (int nt = 0; nt < NUM_NONTERMINALS; nt++) {
        for (int t = 0; t < NUM_TOKENS; t++) {
            int p = tabela_analise[nt][t];
            if (p >= 0) {
                printf("M[%s, %s] = ", nonterm_name(nt), token_name(t));
                imprimir_producao(p);
                printf("  (p=%d)\n", p);
            }
        }
    }
    printf("==============================================================================\n");
}

// Funcao principal do programa.
int main() {
    inicializar_producoes();
    calcular_anulaveis();
    calcular_conjuntos_first();
    calcular_conjuntos_follow();
    construir_tabela_analise_ll1();
    if (!analisar()) {
        // Retorna 1 se houve erro sintatico.
        return 1;
    }
    // Se a analise foi bem-sucedida, lista os tokens processados.
    printf("================================ TOKENS LIDOS ===============================\n");
    for (int i = 0; i < quantidade_tokens_lidos; i++) {
        printf("Token: %-12s Lexema: %s\n",
               token_name(tokens_armazenados[i].tipo),
               tokens_armazenados[i].lexema);
    }
    printf("=============================================================================\n");
    /* Visualizacao*/
/* Visualizacao*/
    imprimir_conjuntos_first();
    imprimir_conjuntos_follow();
    imprimir_tabela_analise();
    
    return 0;
}