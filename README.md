# Projeto de Implementação de Autômatos e Compiladores - Análise Léxica e Análise Sintática LL(1)

Este repositório contém o código-fonte de um Analisador Sintático (Parser) implementado em C, utilizando a técnica LL(1) para validar a sintaxe de uma linguagem de programação simples.
O projeto demonstra os fundamentos da construção de compiladores, incluindo o cálculo de conjuntos FIRST e FOLLOW e a geração da Tabela de Análise Sintática.
Como Funciona:
O parser LL(1) opera de forma preditiva, usando uma pilha e uma tabela de análise.

Pré-cálculo: O programa calcula e constrói a Tabela de Análise Sintática (tabela_analise) a partir das regras da gramática.

Análise: Durante a execução, o parser compara o símbolo no topo da pilha (o que é esperado) com o token de entrada (lookahead).

Decisão: Utiliza a tabela para decidir qual regra de produção aplicar para expandir Não-Terminais.

Resultado: Se a pilha e a entrada se esgotarem simultaneamente, o programa é considerado sintaticamente correto.

## Como exetudar o Projeto
Execute em um ambiente Linux:
```bash
sudo apt update
sudo apt install build-essential flex

## Dentro da pasta do projeto:
flex lexer.l
gcc lex.yy.c parser.c -o analisador -lfl
./analisador < teste.cmini

## Se a análise estiver correta, aparecerá:
Sucesso: programa sintaticamente correto.


