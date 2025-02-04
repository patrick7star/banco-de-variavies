
importa-depedencias:
	@mkdir -p lib/include/
	@cp -uv ../utilitarios/bin/static/libconversao.a lib/
	@cp -uv ../utilitarios/include/conversao.h lib/include/
	@echo "Biblioteca de 'conversão' importada."
	@cp -uv ../utilitarios/bin/static/libteste.a lib/
	@cp -uv ../utilitarios/include/teste.h lib/include/
	@echo "Biblioteca de 'teste' importada."
	@cp -uv ../utilitarios/bin/static/libtempo.a lib/
	@cp -uv ../utilitarios/include/tempo.h lib/include/
	@echo "Biblioteca de 'tempo' importada."
	@cp -uv ../utilitarios/bin/static/liblegivel.a lib/
	@cp -uv ../utilitarios/include/legivel.h lib/include/
	@echo "Biblioteca de 'legível' importada."
	@cp -uv ../utilitarios/bin/static/libterminal.a lib/
	@cp -uv ../utilitarios/include/terminal.h lib/include/
	@echo "Biblioteca de 'terminal' importada."
	@cp -uv ../utilitarios/bin/static/libaleatorio.a lib/
	@cp -uv ../utilitarios/include/aleatorio.h lib/include/
	@echo "Biblioteca de 'aleatório' importada."
	@cp -uv ../utilitarios/bin/static/libhtref.a lib/
	@cp -uv ../utilitarios/include/hashtable_ref.h lib/include/
	@echo "Biblioteca de 'hashtable-ref' importada."
	@cp -uv ../utilitarios/include/definicoes.h lib/include/
	@echo "Biblioteca de 'definicoes.h' importada."

objeto-banco:
	@mkdir -p build/
	clang -c -Wall -std=gnu2x -I../utilitarios/include \
		-o build/banco.o banco.c

teste-dados:
	# Criando o diretório para o testes:
	@mkdir -p bin/tests/
	# Compilando...
	clang -Wall -std=gnu2x -I lib/include \
		-D__unit_tests__ -D__debug__ \
		-o bin/tests/ut_dados dados.c -lm \
		-L lib/ -lteste -ltempo -llegivel -lterminal -laleatorio

teste-tabela:
	# Criando o diretório para o testes:
	@mkdir -p bin/tests/
	# Compilando...
	clang -Wall -std=gnu2x -I lib/include \
		-D__unit_tests__ -D__debug__ \
		-o bin/tests/ut_tabela tabela.c -lm \
		-L lib/ -lteste -ltempo -llegivel -lterminal -lhtref

teste-banco:
	# Criando o diretório para o testes:
	@mkdir -p bin/tests/
	# Compilando...
	clang -c -Wall -I lib/include -D__unit_tests__ -D__debug__ \
		-o build/banco-teste.o banco.c
	clang -Wall -std=gnu2x -I lib/include -L./lib/ \
		-D__unit_tests__ -D__debug__ \
		-o bin/tests/ut_banco build/banco-teste.o \
		-lm -lteste -ltempo -llegivel -lterminal -lhtref -laleatorio

teste-serializacao:
	clang -c -Ilib/include -Wall -Werror -D__debug__ -o build/dados.o dados.c
	clang -c -Ilib/include -Wall -Werror -o build/tabela.o tabela.c
	clang -c -Ilib/include -Wall -std=gnu2x \
		-D__debug__ -D__unit_tests__ \
		-o build/serializacao-teste.o serializacao.c
	clang -g -ggdb -I../utilitarios/include -Wall -std=gnu2x \
		-o bin/tests/ut_serializacao \
		build/dados.o build/tabela.o build/serializacao-teste.o \
		-Llib/ -lconversao -lteste -ltempo -llegivel -lterminal \
		-laleatorio -lm -lhtref

