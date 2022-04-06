#include <iostream>
#include <fstream>
#include <cstring>
#include <string.h>
#include "parser.h"
#include "error.h"
using namespace std;

// arquivo de entrada
ifstream fin;

void messageHelp();
void messageVersion();

// programa pode receber nomes de arquivos
int main(int argc, char **argv)
{
	if (argc == 2)
	{	
		// --help ou --version
		if (argv[1][0] == '-')
		{
			if (!strcmp(argv[1], "--help") || !strcmp(argv[1], "-H"))
				messageHelp();

			else if (!strcmp(argv[1], "--version") || !strcmp(argv[1], "-V"))
				messageVersion();

			else cout << "Comando inválido, use \"tradutor --help\" para descobrir os comandos válidos.\n";	
			exit(EXIT_FAILURE);
		}

		// verifica se o arquivo tem formato ".math"
		int tam = strlen(argv[1]);
		if (strcmp(&argv[1][tam-5], ".math"))
		{
			cout << "O arquivo precisa ter a extensão \".math\".\n\n";
			exit(EXIT_FAILURE);
		}

		fin.open(argv[1]);

		if (!fin.is_open())
		{
			cout << "Falha na abertura do arquivo \'" << argv[1] << "\'.\n";
			cout << "Verifique se o nome está correto, ou se é protegido para leitura.\n";
			exit(EXIT_FAILURE);
		}

		Parser tradutor;
		try
		{
			tradutor.Start();
		}
		catch (SyntaxError err)
		{
			err.What();
		}

		cout << endl;
		fin.close();
	}
	else
	{
		cout << "Falha na execução do tradutor...\n";
		cout << "Use \"tradutor --help\" para saber como utilizar-lo.\n\n";
		exit(EXIT_FAILURE);
	}
}

void messageHelp()
{
	cout << 
	"Uso\n\n" <<

	"	tradutor <nome-do-arquivo>\n\n" <<

	"O arquivo que irá ser traduzido precisa está no formato '.math'.\n\n" <<

	"Opções\n\n" <<

	"	--help, -H		Imprima as informações de uso e saia.\n" <<
  	"	--version, -V		Imprima o número da versão e saia.\n\n" <<

	"Para mais informações sobre o tradutor, consulte README.md em \"https://github.com/felpsribeiro/tradutor-expressoes\".\n";
}

void messageVersion()
{
	cout << "tradutor versão 1.0.0\n\n";
}
