# Caminho a biblioteca externa:
HEADER 	 = -I$(CCODES)/utilitarios-em-c/include
LIB_UTILS = $(CCODES)/utilitarios-em-c
LIB 	 	 = $(LIB_UTILS)/bin/shared
TOSTER 	 = -L$(LIB) -lconversao -lteste -ltempo -llegivel -lterminal \
				-laleatorio -lm -lhtref
TESTER 	 = -L$(LIB) -lteste -ltempo -llegivel -lterminal -laleatorio -lm

importa-depedencias:
	@mkdir -p lib/include/
	@cp -uv $(LIB_UTILS)/bin/static/libconversao.a lib/
	@cp -uv $(LIB_UTILS)/include/conversao.h lib/include/
	@echo "Biblioteca de 'conversão' importada."
	@cp -uv $(LIB_UTILS)/bin/static/libteste.a lib/
	@cp -uv $(LIB_UTILS)/include/teste.h lib/include/
	@echo "Biblioteca de 'teste' importada."
	@cp -uv $(LIB_UTILS)/bin/static/libtempo.a lib/
	@cp -uv $(LIB_UTILS)/include/tempo.h lib/include/
	@echo "Biblioteca de 'tempo' importada."
	@cp -uv $(LIB_UTILS)/bin/static/liblegivel.a lib/
	@cp -uv $(LIB_UTILS)/include/legivel.h lib/include/
	@echo "Biblioteca de 'legível' importada."
	@cp -uv $(LIB_UTILS)/bin/static/libterminal.a lib/
	@cp -uv $(LIB_UTILS)/include/terminal.h lib/include/
	@echo "Biblioteca de 'terminal' importada."
	@cp -uv $(LIB_UTILS)/bin/static/libaleatorio.a lib/
	@cp -uv $(LIB_UTILS)/include/aleatorio.h lib/include/
	@echo "Biblioteca de 'aleatório' importada."
	@cp -uv $(LIB_UTILS)/bin/static/libhtref.a lib/
	@cp -uv $(LIB_UTILS)/include/hashtable_ref.h lib/include/
	@echo "Biblioteca de 'hashtable-ref' importada."
	@cp -uv $(LIB_UTILS)/include/definicoes.h lib/include/
	@echo "Módulo 'definicoes.h' importada."
	@cp -uv $(LIB_UTILS)/include/macros.h lib/include/
	@echo "Módulo 'macros.h' importado."
	@cp -uv $(LIB_UTILS)/include/memoria.h lib/include/
	@cp -uv $(LIB_UTILS)/bin/static/libmemoria.a lib/
	@echo "Biblioteca de 'memória' importada."

# ~~~ ~~~ ~~~ ~~~ ~~~ ~~~ ~~~ ~~~ ~~~ ~~~ ~~~ ~~~ ~~~ ~~~ ~~~ ~~~ ~~~ ~~~ ~
objeto-banco:
	@mkdir -p build/
	clang -c -Wall -std=gnu2x -I../utilitarios/include \
		-o build/banco.o banco.c

# ~~~ ~~~ ~~~ ~~~ ~~~ ~~~ ~~~ ~~~ ~~~ ~~~ ~~~ ~~~ ~~~ ~~~ ~~~ ~~~ ~~~ ~~~ ~
obj-dados:
	@clang -Wall -Werror -std=gnu2x $(HEADER) -c -o build/dados.o dados.c
	@echo "Objeto de 'dados' compilado."

obj-test-dados:
	@clang -Wall -Werror -std=gnu2x $(HEADER) \
		-c -o build/dados-teste.o dados.c
	@echo "Objeto do teste de 'dados' compilado."

test-dados: obj-test-dados
	# Criando o diretório para o testes:
	@mkdir -p bin/tests/
	# Compilando...
	clang -Wall -std=gnu2x $(HEADER) -D__unit_tests__ -D__debug__ \
		-o bin/tests/ut_dados dados.c $(TESTER) -laleatorio -lmemoria

# ~~~ ~~~ ~~~ ~~~ ~~~ ~~~ ~~~ ~~~ ~~~ ~~~ ~~~ ~~~ ~~~ ~~~ ~~~ ~~~ ~~~ ~~~ ~
obj-tabela:
	@clang $(HEADER) -Wall -Werror -c -o build/tabela.o tabela.c

test-tabela:
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

# ~~~ ~~~ ~~~ ~~~ ~~~ ~~~ ~~~ ~~~ ~~~ ~~~ ~~~ ~~~ ~~~ ~~~ ~~~ ~~~ ~~~ ~~~ ~
obj-test-serializacao:
	@clang -g -Ilib/include -Wall -std=gnu2x -D__debug__ -D__unit_tests__ \
		-c -o build/serializacao-teste.o serializacao.c

#clang -c -Ilib/include -Wall -Werror -D__debug__ -o build/dados.o dados.c
test-serializacao:
	@clang $(HEADER) -o bin/tests/ut_serializacao build/serializacao-teste.o \
		build/dados.o build/tabela.o \
		$(TESTER) -limpressao -laleatorio -lconversao -lhtref
	@echo "Teste de 'serialização' compilado."

