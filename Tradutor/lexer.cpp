#include "lexer.h"
#include <fstream>
#include <sstream>
#include <math.h>
using std::stringstream;

extern std::ifstream fin;

// construtor
Lexer::Lexer()
{
	// insere palavras-reservadas na tabela de id's
	id_table["int"] = Id{Tag::TYPE, "int"};
	id_table["float"] = Id{Tag::TYPE, "float"};
	id_table["math"] = Id{Tag::FUNC_INI, "math"};

	// inicia leitura da entrada
	peek = fin.get();
}

// retorna número da linha atual
int Lexer::Lineno()
{
	return line;
}

void Lexer::Clean()
{
	char nextPeek;
	while (isspace(peek) || peek == '/')
	{
		// salta comentários
		if (peek == '/')
		{
			// espia o próximo caracter
			nextPeek = fin.peek();

			// comentários de uma linha -> // ....
			if (nextPeek == '/')
			{
				peek = fin.get();
				while (peek != '\n' && peek != EOF)
					peek = fin.get();
				// se achar o final do arquivo, encerra a função
				if (peek == EOF)
					return;
				else if (peek == '\n')
					line += 1;
			}

			// comentários de mais de uma linha -> /* ..... */
			else if (nextPeek == '*')
			{
				peek = fin.get();
				bool flag = true;
				do
				{
					peek = fin.get();

					switch (peek)
					{
					case '\n':
						line += 1;
						break;

					// se achar o final do arquivo, encerra a função
					case EOF:
						return;

					case '*':
						nextPeek = fin.peek();
						if (nextPeek == '/')
						{
							peek = fin.get();
							flag = false;
						}
						break;

					default:
						break;
					}
				} while (flag);
				peek = fin.get();
			}
			// '/' representa uma divisão
			else return;
		}

		// salta espaços em branco, tabulações e novas linhas
		if (isspace(peek))
		{
			if (peek == '\n')
				line += 1;
			peek = fin.get();
		}
	}
}

// retorna tokens para o analisador sintático
Token *Lexer::Scan()
{
	// ignora espaços em branco, tabulações e novas linhas
	Clean();

	// retorna números
	if (isdigit(peek))
	{
		int acumulado = 0;
		int casasDecimas = 0;
		bool flagDecimas = false;

		do
		{
			// converte caractere 'n' para o dígito numérico n
			int n = peek - '0';

			acumulado = 10 * acumulado + n;
			if (flagDecimas) 
				++casasDecimas;

			peek = fin.get();
			if (peek == '.' && !flagDecimas)
			{
				flagDecimas = true;
				peek = fin.get();
			}
		} while (isdigit(peek));

		// retorna o token NUM float
		if (flagDecimas) {
			token.real = Float{(float)(acumulado / pow(10, casasDecimas))};
			return &token.real;
		}

		// retorna o token NUM int
		else
		{
			token.inteiro = Int{acumulado};
			return &token.inteiro;
		}
	}

	// retorna palavras-chave e identificadores
	if (isalpha(peek))
	{
		stringstream ss;
		do
		{
			ss << peek;
			peek = fin.get();
		} while (isalpha(peek));

		string s = ss.str();
		auto pos = id_table.find(s);

		// se o lexema já está na tabela
		if (pos != id_table.end())
		{
			// retorna o token associado
			token.word = pos->second;
			return &token.word;
		}

		// se o lexema ainda não está na tabela
		Id new_id{Tag::ID, s};
		id_table[s] = new_id;

		// retorna o token ID
		token.word = new_id;
		return &token.word;
	}

	// operadores (e caracteres não alphanuméricos isolados)
	Token op{peek};
	peek = ' ';

	// retorna o token
	token.simbolo = op;
	return &token.simbolo;
}
