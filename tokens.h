#ifndef TOKENS_H
#define TOKENS_H

// Todos os tipos de tokens da sua linguagem
typedef enum {
    T_EOF = 0,     // fim da entrada (0 reservado pro EOF)

    T_MAIN,        // 1
    T_TIPO,        // 2  -> int, float, char, void

    T_IF,          // 3
    T_ELSE,        // 4
    T_WHILE,       // 5
    T_DO,          // 6
    T_FOR,         // 7
    T_RETURN,      // 8
    T_READ,        // 9
    T_PRINT,       // 10

    T_PV,          // 11  ;
    T_VIRG,        // 12  ,
    T_IGUAL,       // 13  =
    T_PA,          // 14  (
    T_PF,          // 15  )
    T_CA,          // 16  {
    T_CF,          // 17  }
    T_SOMA,        // 18  +
    T_SUB,         // 19  -
    T_MUL,         // 20  *
    T_DIV,         // 21  /

    T_OP_COM,      // 22  == != > < >= <=
    T_OP_LOG,      // 23  && ||
    T_NOT,         // 24  !

    T_ID,          // 25  identificadores
    T_NUM,         // 26  números

    T_ERROR        // 27  erro léxico
} TokenType;

#define NUM_TOKENS 28

#endif
