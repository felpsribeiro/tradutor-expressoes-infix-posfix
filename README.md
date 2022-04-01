# Tradutor de expressões infixadas para pós-fixadas

Implementação de um tradutor que ler um arquivo .math contendo expressões aritméticas em notação infixada, descritas de acordo com a gramática abaixo, e as traduz para uma notação pós-fixada. Esse projeto é a atividade avaliativa da unidade 1 da disciplina de Compiladores do professor [Judson Santiago](https://www.youtube.com/playlist?list=PLX6Nyaq0ebfhI396WlWN6WlBm-tp7vDtV).

## Gramática
    program → math block
    block   → { decls stmts }
    decls   → decls decl
            | ϵ
    decl    → type id;
    stmts   → stmts stmt
            | ϵ
    stmt    → block
            | expr;
    expr    → expr + term
            | expr – term
            | term
    term    → term * factor
            | term / factor
            | factor
    facto   → (expr)
            | num
            | id

## Esquema de Tradução Dirigido por Sintaxe
~~~bnf
program ::= math block

block   ::= { decls stmts }

decls   ::= decl decls
          | NULL

decl    ::= type id

stmts   ::= stmt stmts
          | NULL

stmt    ::= block
          | expr

expr    ::= term opexpr

opexpr  ::= + term opexpr {print('+');}
          | – term opexpr {print('-');}
          | NULL

term    ::= factor opterm

opterm  ::= * factor opterm {print('*');}
          | / factor opterm {print('/');}
          | NULL

factor  ::= (expr)
          | num {printf('(%d)', num.valor);}
          | id {printf('(%s:%s)', id.name, id.type);}
~~~


## Exemplo de tradução
### Entrada - _arquivo.math_
~~~c++
math
{ 
    int x;
    int val;
    x + val;
    2+(3-5);
    { 
        float x; /* outro x */
        2 – 3.5 + x;
    } 
    x;
}
~~~

### Saída - _terminal do bash_
~~~shell
(x:int)(val:int)+
(2)(3)(5)-+
(2)(3.5)-(x:float)+
(x:int)
~~~

## Estrutura do Tradutor
Componente | Arquivo | Função
|----------|---------|---
Tradutor | tradutor.cpp | inicializa o esquema de tradução
Analizador Léxico | lexer.cpp | lê os caracteres dos arquivos
Analizador Sintático | parser.cpp | verifica se a cadeia recebida está de acordo com a gramática
Tabela de Símbolos | symtable.cpp | tabela hash que armazena palavras reservadas e nomes de variáveis
Gerenciador de Erros | error.cpp | gerência os erros
------------
## Como utilizar
1. Instale o compilador g++, o depurador gdb, o o sistema de automação da compilação make e cmake
~~~shell
$ sudo apt update
$ sudo apt upgrade
$ sudo apt install g++ gdb make cmake
~~~
2. Clone este repositorio em sua máquina;

### para versão Release...

3. Abra o projeto no Bash e rode o CMake para gerar o Makefile dentro da pasta Release, digitando
~~~shell
$ mkdir Release
$ cd Release/
/Release$ cmake ../ -DCMAKE_BUILD_TYPE=Release
~~~
4. Agora rode o Makefile para gerar seu arquivo executável
~~~shell
/Release$ make
~~~
5. Para rodar o arquivo 'teste.math' vá no terminal e digite
~~~shell
/Realese$ ./tradutor ../teste.math
~~~

### para versão Debug...

3. Abra o projeto no Bash e rode o CMake para gerar o Makefile dentro da pasta Dedug, digitando
~~~shell
$ mkdir Debug
$ cd Debug/
/Debug$ cmake ../ -DCMAKE_BUILD_TYPE=Debug
~~~
4. Agora rode o Makefile para gerar seu arquivo executável
~~~shell
/Debug$ make
~~~
5. Para rodar o arquivo 'teste.math' vá no terminal e digite
~~~shell
/Debug$ ./tradutor ../teste.math
~~~