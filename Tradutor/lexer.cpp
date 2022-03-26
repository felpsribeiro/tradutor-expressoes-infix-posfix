#include "lexer.h"
#include <fstream>
#include <sstream>
using std::stringstream;

extern std::ifstream fin;

// construtor
Lexer::Lexer()
{
	// insere palavras-reservadas na tabela de id's
	id_table["int"] = Id{Tag::TYPE, "int"};
	id_table["float"] = Id{Tag::TYPE, "float"};

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
				{
					// se achar o final do arquivo, encerra a função
					if (peek == EOF)
						return;
					else if (peek == '\n')
						line += 1;
					peek = fin.get();
				}
			}

			// comentários de mais de uma linha -> /* ..... */
			else if (nextPeek == '*')
			{
				peek = fin.get();
				bool flag = true;
				do
				{
					peek = fin.get();
					// se achar o final do arquivo, encerra a função
					if (peek == EOF)
						return;
						
					else if (peek == '\n')
						line += 1;						

					else if (peek == '*')
					{
						peek = fin.get();
						if (peek == '/')
							flag = false;
					}
				} while (flag);
				peek = fin.get();
			}
		}

		// salta espaços em branco, tabulações e novas linhas
		else
			peek = fin.get();
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
		int v = 0;
		int d = 0;

		do
		{
			// converte caractere 'n' para o dígito numérico n
			int n = peek - '0';

			v = 10 * v + n;
			if (d != 0) 
				++d;

			peek = fin.get();
			if (peek == '.' && d == 0)
				++d;
		} while (isdigit(peek));

		// retorna o token NUM float
		if (d != 0)
		{
			v /= d * 10;
			token.n = Num{v};
		}
		// retorna o token NUM int
		else
			token.n = Num{v};
		
		return &token.n;
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
			token.i = pos->second;
			return &token.i;
		}

		// se o lexema ainda não está na tabela
		Id new_id{Tag::ID, s};
		id_table[s] = new_id;

		// retorna o token ID
		token.i = new_id;
		return &token.i;
	}

	// operadores (e caracteres não alphanuméricos isolados)
	Token op{peek};
	peek = ' ';

	// retorna o token
	token.t = op;
	return &token.t;
}