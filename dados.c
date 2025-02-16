
// Declaração das estruturas, funções e métodos abaixo:
#include "dados.h"
#include "macros.h"
#include "aleatorio.h"
#include "memoria.h"
// Bibliotecas do C padrão, necessários para este arquivo:
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdarg.h>
#include <wchar.h>

/* === === === === === === === === === === === === === === === === === ==
 *                            Estringue usada aqui
 *                         Semelhante à 'string_view'
 *                                  do C++
 * === === === === === === === === === === === === === === === === === ==*/
// Estrutura que ármazena uma estringue Unicode, e o total de caractéres.
struct String new_string(wchar_t* In) { 
   return (struct String){ box_wcs(In), wcslen(In) };
}

/* === === === === === === === === === === === === === === === === === ==
 *                         Enumerador que Identifica
 *                            o Primitivo que o 
 *                         Registro Representa.
 * === === === === === === === === === === === === === === === === === ==*/
// Pega um Enum e faz do nome dele uma string.
#define STR(X) #X

// Todos tipos de dados, com seus respectívos códigos, aceito na fabricação
// de um registro para o 'banco de variáveis'.

void enum_invalido(void)
{
   perror("tipo de dado inválido");
   abort();
}

const char* dp_to_str(Tipo a)
{
/* Converte o 'enum' acima para a forma estringue, se for um 'inteiro'
 * válido(correspondente aos 'enum' dados). */
   switch(a) {
      case Decimal:
         return STR(Decimal);
      case Caractere:
         return STR(Caractere);
      case Logico:
         return STR(Logico);
      case Inteiro:
         return STR(Inteiro);
      case Texto:
         return STR(Texto);
      default:
         enum_invalido();
   }
}

Tipo random_dp(void)
{
// Retorno um enumerador do 'Tipo' randômico.
   int X = inteiro_positivo(95, 99);
   return (Tipo)((-1) * X);
}

/* === === === === === === === === === === === === === === === === === ==
 *                         Valor que um Registro
 *                            Carrega no Banco
 *
 *   Uma união, com todos os tipos primitivos -- a string unicode sendo a 
 * única exceção. Como cada valor no registro é só de um tipo, a estrutura
 * 'união' é perfeita para isso, economiza banstante memória.
 * === === === === === === === === === === === === === === === === === ==*/

union Value new_value(Tipo tipo_de_dado, void* vl)
{
/* Converte atual pointeiro para algum dado na 'união Value' para a 
 * determinada união. */
   union Value valor;

   switch (tipo_de_dado) 
   {
      case Inteiro:
         valor.inteiro = *((int64_t*)vl);
         break;

      case Decimal:
         valor.decimal = *((double*)vl);
         break;

      case Texto:
         // valor.texto = *((struct String*)vl);
         // Realiza uma cópia, e a coloca na 'heap'.
         valor.texto = new_string((wchar_t*)vl);
         break;

      case Caractere:
         valor.caractere = *((wchar_t*)vl);
         break;

      case Logico:
         valor.logico = *((bool*)vl);
         break;

      default:
         enum_invalido();
   }
   return valor;
}

wchar_t* value_to_str(Tipo tipo, union Value* ptr)
{
/* Converte o atual tipo de dado numa string unicode. É Unicode, porque 
 * um dos dados da 'união valor' também transporta uma estringue Unicode,
 * logo sua versão string também pode ser uma, ou apenas usar estringue
 * ASCII faria uma tarefa intediante converte-la toda hora para o tipo
 * imprimivel. */
   const int T = 300, sz = sizeof(wchar_t);
   wchar_t* fmt = malloc(T * sz);
   // Variáveis usados dentro do 'switch'.

   switch (tipo) {
      case Inteiro:
         swprintf(fmt, T, L"%ld", (*ptr).inteiro);
         break;

      case Decimal:
         swprintf(fmt, T, L"%lf",ptr->decimal);
         break;

      case Texto:
         swprintf(
            fmt, T, L"\"%ls\" [%zu]", 
            ptr->texto.array, ptr->texto.len
         );
         break;

      case Caractere:
         swprintf(fmt, T, L"'%lc'", ptr->caractere);
         break;

      case Logico:
         swprintf(fmt, T, L"%s", bool_to_str(ptr->logico));
         break;

      default:
         enum_invalido();
   }
   return fmt;
}

union Value random_value(Tipo* tipo)
{
/* Geração aleatório, com valores aleatórios obviamente, do tipo 
 * 'union Value'. Este tipo de coisa é mais adequado para debugging, ou 
 * seja, criação em série de dados tipos tipo para ver se funciona. Também
 * informa o tipo que foi selecionado, passando ela para o endereço que
 * foi dado como argumento. */
   union Value output;
   char letra_ascii, *src;
   wchar_t letra_unicode, *dst;
   float a;
   int64_t x;

   switch(random_dp()) 
   {
      case Decimal:
         *tipo = Decimal;
         a = (float)inteiro_positivo(1, 100);
         output.decimal = a / 100.0f;
         break;

      case Inteiro:
         *tipo = Inteiro;
         x = inteiro_positivo(1, 30);
         if (logico())
            output.inteiro = (-1) * x;
         else
            output.inteiro = x;
         break;

      case Caractere:
         letra_ascii = alfabeto_aleatorio();
         const int sz_char = sizeof(char);

         assert(mbtowc(&letra_unicode, &letra_ascii, sz_char) != -1);
         *tipo = Caractere;
         output.caractere = letra_ascii;
         break;

      case Logico:
         output.logico = logico();
         *tipo = Logico;
         break;

      case Texto:
         src = palavra_aleatoria(); 
         const int n = 30, sz = sizeof(wchar_t);
         dst = malloc(n * sz); 

         *tipo = Texto;
         mbstowcs(dst, src, n);
         output.texto = (struct String){ .array=dst, .len=wcslen(dst) };
         break;
   }
   return output;
}

void free_value(Tipo t, union Value* obj) 
{ 
   switch(t) {
      case Texto:
         free(obj->texto.array);
         break;

      default:
         printf(
            "['%s' | linha: %d] Não possíve liberar o tipo '%s'.\n", 
            __FILE__, __LINE__,  dp_to_str(t)
         );
   }
}

/* === === === === === === === === === === === === === === === === === ==
 *                         Entrada de dados para
 *                      a HashTable que representa o Banco
 *
 *    Ela não é só o valor da entrada na HashTable, também têm a referência
 * da chave que ela segura para identifica-la.
 * === === === === === === === === === === === === === === === === === ==*/
Registro cria_registro(Tipo t, wchar_t* k, void* vl)
{
/* Cria um simples registro, onde o tempo de criação e atualização é pego
 * durante a instanciação. */
   // Selo de tempo de hoje:
   time_t momento;
   // Obtendo valor...
   time(&momento);

   return (Registro) {
      // Realiza uma cópia da estringue passada.
      .chave = new_string(k),
      .valor = new_value(t, vl),
      .criacao = momento,
      .atualizacao = momento,
      .tipo = t
   }; 
}

Registro cria_registro_direto(Tipo type, ...)
{
/* O mesmo que acima, inclusive utiliza da chamada dele para criar, só que,
 * muito mais flexível nos parâmetros. Apenas com o tipo dado, é sabido 
 * que mais dois são necessários, um será um ponteiro de caractére Unicode, 
 * e o outro o valor do tipo. Isso mata a charada para o que converter. */
   va_list lista;

   va_start(lista, type);
   wchar_t* chave = va_arg(lista, wchar_t*);
   wchar_t* s;
   union Value valor;

   switch(type) 
   {
      case Inteiro:
         valor.inteiro = va_arg(lista, int); 
         if (valor.inteiro > INT_MAX)
         {
            printf("Não funciona para inteiros maiores que 'int'.");
            abort();
         }
         break;

      case Decimal:
         valor.decimal = va_arg(lista, double); 
         break;

      case Caractere:
         valor.caractere = va_arg(lista, wchar_t); 
         break;

      case Texto:
         // valor.texto = (struct String){ .array=s, .len=t };
         s = va_arg(lista, wchar_t*);
         valor.texto = new_string(s);
         break;

      case Logico:
         valor.logico = va_arg(lista, int);
         break;

      default:
         perror("tipo de dado inválido");
         abort();
   }
   va_end(lista);

   return (Registro) {
      // Realiza uma cópia da string passada(tem que ser desalocada).
      .chave = new_string(chave),
      .valor = valor,
      .criacao = time(NULL),
      .atualizacao = time(NULL),
      .tipo = type
   }; 
}

void free_registro(Registro* reg)
{ 
/* Única parte alocada dinamicamente em toda estrutura a retomada pelo
 * sistema. */

   if (reg->tipo == Texto) 
   /* Se for do tipo 'texto', então liberar a estringue interna no 'valor'
    * também, já que também foi copiado prá 'heap'. */
   switch(reg->tipo) {
      case Texto:
         free(reg->valor.texto.array); 
         break;
      default:
         printf(
            "['%s' | linha: %d]Tipo(%d): %s\n", 
            __FILE__, __LINE__, reg->tipo, dp_to_str(reg->tipo)
         );
   }
   // Chave também retém uma string alocada(foi copiada na 'heap').
   free(reg->chave.array); 
}

Registro random_registro(void)
{
/* Cria um registro de forma aleatório, com valores aleatórios. Ferramenta
 * é útil para debugging. */
   Registro output;
   Tipo tp;
   union Value e = random_value(&tp);
   time_t c = inteiro_positivo(100, 499);
   time_t a = inteiro_positivo(500, 999);
   struct String k;
   const int sz = sizeof(wchar_t);
   char* p = palavra_aleatoria();
   int t = strlen(p);
   wchar_t* pU = malloc((t + 1) * sz);

   mbstowcs(pU, p, t);
   k = (struct String) { pU, t };
   k.array[t] = L'\0';

   output.tipo = tp;
   output.chave = k;
   output.valor = e;
   output.criacao = c;
   output.atualizacao = a;

   return output;
}

void debug_registro(Registro* obj)
{
/* Mostra um registro, como seus campos de forma identada e organizada.
 * Tal ferramenta também é essencial para debuggin'. */
   Tipo tp = obj->tipo;
   union Value* vp = &obj->valor;
   wchar_t* valor_fmt = value_to_str(tp, vp);
   const char* tipo_fmt = dp_to_str(tp);

   printf(
    "Registro {\n\t.chave: \t\"%ls\"[%lu]\n\t.valor: \t{%ls}\n\t.tipo:%17s"
    "\n\t.criação:\t%lu seg\n\t.atualização:\t%lu seg\n}\n",
    obj->chave.array, obj->chave.len, valor_fmt, tipo_fmt, obj->criacao,
    obj->atualizacao
   );
   free(valor_fmt);
}

#ifdef __unit_tests__
/* === === === === === === === === === === === === === === === === === ==
 *                            Testes Unitários
 *    Faz uma série de testes dos dados estruturados, e as funções que o
 * seguem(métodos) acima.
 * === === === === === === === === === === === === === === === === === ==*/
#include "teste.h"
#include <locale.h>

static void visualizacao_do_registro(Registro* r)
{
   Tipo type = r->tipo;
   void* value = &r->valor;
   wchar_t* valuestr = value_to_str(type, value);

   printf(
      "Registro[\"%ls\" | %s]: (%ls, %zu, %zu)\n",
      r->chave.array, dp_to_str(r->tipo), valuestr,
      r->criacao, r->atualizacao
   );
   free(valuestr);
}

void conversao_de_struct_value_em_string(void)
{
   union Value a, b, c;
   c.inteiro = -150; 
   a.logico = false;
   b.texto = (struct String){L"coragem", 7};
   // b.texto = L"coragem";


   printf(
      "a: %ls\nb: %ls\nc:%ls\n", 
      value_to_str(Logico, &a), 
      value_to_str(Texto, &b),
      value_to_str(Inteiro, &c)
   );
}

void visualizacao_de_instancias_de_registros(void)
{
   puts("Criação padrão dos dados: ");

   int64_t va = -1234567;
   wchar_t vb[] = L"cinza";
   double vc = 3.1415989323;
   bool vd = false;

   Registro a = cria_registro(Inteiro, L"Transição de Imagem", &va);
   Registro b = cria_registro(Texto, L"Cor Do Cadeado", &vb);
   Registro c = cria_registro(Decimal, L"pi", &vc);
   Registro d = cria_registro(Logico, L"o céu está azul", &vd);

   visualizacao_do_registro(&a);
   visualizacao_do_registro(&b);
   visualizacao_do_registro(&c);
   visualizacao_do_registro(&d);

   printf("Sua desalocação ...");
   free_registro(&d);
   free_registro(&b);
   free_registro(&c);
   free_registro(&a);
   puts("feita.");
}

void nova_formar_de_instanciar_reg(void)
{
   puts("Criação direta de um registro(sem referências):");

   Registro regs[] = {
      cria_registro_direto(Inteiro, L"Saldo", -13),
      cria_registro_direto(Caractere, L"gênero", L'F'),
      cria_registro_direto(Logico, L"acionado", true),
      cria_registro_direto(Decimal, L"constante(e)", 3.71),
      cria_registro_direto(Texto, L"acessório", L"relógio")
   };

   for (int i = 1; i <= 5; i++)
      visualizacao_do_registro(&regs[i - 1]);


   printf("Sua desalocação... ");
   fflush(stdout);

   for (int i = 1; i <= 5; i++) 
   {
      printf("%dº, ", i);
      fflush(stdout);
      free_registro(&regs[i - 1]);
   }
   puts("feita");
}

void tamanho_de_cada_tipo(void)
{
   printf(
      "Tamanho de todos tipos de dados deste módulo:\n\n"
      "union Value: %lu bytes.\nRegistro(struct): %lu bytes\n"
      "struct String: %lu bytes\nTipo(enum): %lu bytes\n",
      sizeof(union Value), sizeof(Registro), sizeof(struct String),
      sizeof(Tipo)
   );
}

void gerador_randomico_de_cada_tipo_de_dado(void)
{
   puts("Geração randômica de enumeradores do 'Tipo':");
   for (int i = 1; i <= 10; i++)
   {
      Tipo X = random_dp();
      const char* fmt = dp_to_str(X);

      printf("\t- %-15s (%d)\n", fmt, X);
   }

   puts("\nGeração aleatório de 'union Value\'s'...");
   for (int i = 1; i <= 5; i++)
   {
      Tipo Z;
      union Value Y = random_value(&Z);
      wchar_t* fmt_a = value_to_str(Z, &Y);
      const char* fmt_b = dp_to_str(Z);
      
      printf("\t- %ls, %30s\n", fmt_a, fmt_b);
      free(fmt_a);
   }

   puts("\nGeração aleatório de 'Registros'...");
   for (int i = 1; i <= 3; i++)
   {
      Registro obj = random_registro();
      debug_registro(&obj);
      free_registro(&obj);
   }
}

int main(void)
{
   setlocale(LC_CTYPE, "en_US.UTF-8");
   puts("\nTestes específicos do arquivo 'dados.c'...\n");

   executa_testes_a(
      // Testes referentes ao "módulo" dados.c.
      true, 5, 
         conversao_de_struct_value_em_string, true,
         visualizacao_de_instancias_de_registros, true,
         nova_formar_de_instanciar_reg, true,
         tamanho_de_cada_tipo, true,
         gerador_randomico_de_cada_tipo_de_dado, true
   );
   return EXIT_SUCCESS;
}
#endif
