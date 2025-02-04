
#ifndef _DADOS_H
#define _DADOS_H
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

 // Estrutura que ármazena uma estringue Unicode, e o total de caractéres.
 struct String { wchar_t* array; size_t len; };
 // Estrura que é o valor que uma chave da 'tabela' armazena.
 typedef struct entrada_da_grande_tabela Registro;
 // Todos tipos de dados, com seus respectívos códigos, aceito na fabricação
 // de um registro para o 'banco de variáveis'.
 typedef enum  tipos_de_dados_na_tabela { 
   Inteiro = -99, 
   Texto, 
   Decimal, 
   Caractere, 
   Logico 
 } DadoPropriedade, enumDP;

 union  Value {
   // O valor pode assumir um caractére(símbolo Unicode)...
   wchar_t caractere;

   // Um valor decimal(o maior)...
   double decimal;

   // Uma estringue(estrutura com bytes e comprimento)...
   struct String texto;

   // Um valor booleano...
   bool logico;

   // É o maior inteiro(com sinal) possível.
   int64_t inteiro;
 };

 /* Métodos de geração, ou alteração, das estruturas declaradas acima. Não
  * é claro ainda qual realmente terá um uso global, por enquanto, todas 
  * declaradas, serão, aqui, tornadas públicas. */
 struct String box_string(wchar_t* s);
 const char* dp_to_str(DadoPropriedade a);
 DadoPropriedade random_dp(void);

 union Value new_value (DadoPropriedade tipo, void* vl);
 wchar_t* value_to_str (DadoPropriedade tipo, union Value* ptr);
 union Value random_value (DadoPropriedade* tipo);
 void free_value(DadoPropriedade t, union Value* obj); 

 Registro cria_registro (DadoPropriedade t, wchar_t* k, void* vl);
 Registro cria_registro_direto (DadoPropriedade type, ...);
 void free_registro (Registro* reg);
 void debug_registro (Registro* obj);
 Registro random_registro (void);

#endif

#ifndef _DADOS_ESTRUTURA_H
#define _DADOS_ESTRUTURA_H

// Definição do tipo 'Registro':
struct entrada_da_grande_tabela
{
   /* Estringues representando a chave do banco; também aqui seu 
    * comprimento, porque estringues tem comprimentos. Irá ser um 
    * 'string_view', pois este tipo de dado será muitas vezes carregado 
    * do 'mass storage', logo, a memória estará sendo alocado na 'heap', 
    * apesar de que os exemplos simples aqui, estejam na stack. */
   struct String chave;

   // Pode ser apenas um único valor abaixo.
   union Value valor;

   // Que tipo de objeto isto representa.
   DadoPropriedade tipo;

   // Quando foi criada tal entrada.
   time_t criacao;

   // Última vez que valor corresponde a chave foi alterado.
   time_t atualizacao;
};
#endif
