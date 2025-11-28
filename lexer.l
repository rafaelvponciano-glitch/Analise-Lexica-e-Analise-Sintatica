%option noyywrap

%{
#include <stdio.h>
#include "tokens.h"
char *yy_lexema;
%}

DIGITO      [0-9]
LETRA       [a-zA-Z_]
ID          {LETRA}({LETRA}|{DIGITO})*
NUMERO      {DIGITO}+(\.{DIGITO}+)?

%%
"main"      { yy_lexema = yytext; return T_MAIN; }

"int"       { yy_lexema = yytext; return T_TIPO; }
"float"     { yy_lexema = yytext; return T_TIPO; }
"char"      { yy_lexema = yytext; return T_TIPO; }
"void"      { yy_lexema = yytext; return T_TIPO; }

"if"        { yy_lexema = yytext; return T_IF; }
"else"      { yy_lexema = yytext; return T_ELSE; }
"while"     { yy_lexema = yytext; return T_WHILE; }
"do"        { yy_lexema = yytext; return T_DO; }
"for"       { yy_lexema = yytext; return T_FOR; }
"return"    { yy_lexema = yytext; return T_RETURN; }

"read"      { yy_lexema = yytext; return T_READ; }
"print"     { yy_lexema = yytext; return T_PRINT; }

"=="        { yy_lexema = yytext; return T_OP_COM; }
"!="        { yy_lexema = yytext; return T_OP_COM; }
">="        { yy_lexema = yytext; return T_OP_COM; }
"<="        { yy_lexema = yytext; return T_OP_COM; }
">"         { yy_lexema = yytext; return T_OP_COM; }
"<"         { yy_lexema = yytext; return T_OP_COM; }

"&&"        { yy_lexema = yytext; return T_OP_LOG; }
"||"        { yy_lexema = yytext; return T_OP_LOG; }
"!"         { yy_lexema = yytext; return T_NOT; }

";"         { yy_lexema = yytext; return T_PV; }
","         { yy_lexema = yytext; return T_VIRG; }
"="         { yy_lexema = yytext; return T_IGUAL; }
"("         { yy_lexema = yytext; return T_PA; }
")"         { yy_lexema = yytext; return T_PF; }
"{"         { yy_lexema = yytext; return T_CA; }
"}"         { yy_lexema = yytext; return T_CF; }
"+"         { yy_lexema = yytext; return T_SOMA; }
"-"         { yy_lexema = yytext; return T_SUB; }
"*"         { yy_lexema = yytext; return T_MUL; }
"/"         { yy_lexema = yytext; return T_DIV; }

"//".*      {  }

{NUMERO}    { yy_lexema = yytext; return T_NUM; }
{ID}        { yy_lexema = yytext; return T_ID; }

[ \t\r\n]+  {  }

.           {
                fprintf(stderr,
                        "Erro léxico: caractere inválido '%s'\n",
                        yytext);
                yy_lexema = yytext;
                return T_ERROR;
            }

%%