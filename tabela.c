/* Cria as funções para a hashtable possa inserir, localizar e remover 
 * chaves do tipo estringue adicionadas a elas. Note que, os valores dos
 * dicionários também são estringues Unicode.
 */
#include "tabela.h"
#include <wchar.h>

size_t registro_hash(generico_t k, size_t cp)
{
   wchar_t* key = (wchar_t*)k;
   size_t length = wcslen(key);
   size_t head, middle, tail, index;

   if (length >= 5) {
      head = (size_t)key[0];
      middle = (size_t)(key[length / 2]);
      tail = (size_t)key[length - 1];

   } else if (length >= 20) {
      index = (size_t)((float)length * 0.15);
      head = (size_t)key[index];
      middle = (size_t)key[length / 2];
      index = length - head;
      tail = (size_t)key[index];
   } else {
      head = (size_t)key[0];
      middle = tail = 1;
   }

   return (head * middle * tail) % cp;
}

size_t registro_hash_i(generico_t k, size_t cp)
{
   wchar_t* key = k;
   size_t length = wcslen(key);
   size_t soma = 0;

   for (int i = 1; i <= length; i++)
   {
      size_t a = 2 * i;
      size_t b = (size_t)key[i - 1];
      soma += (a * b);
   }

   return soma % cp;
}

bool registro_eq(generico_t a, generico_t b)
{
   wchar_t* key_a = a;
   wchar_t* key_b = b;
   
   return (wcscmp(key_a, key_b) == 0);
}

#ifdef __unit_tests__
/* === === === === === === === === === === === === === === === === === ==
 *                            Testes Unitários
 *    Faz uma série de testes dos dados estruturados, e as funções que o
 * seguem(métodos) acima.
 * === === === === === === === === === === === === === === === === === ==*/
#include <assert.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include "teste.h"

void visualiza_hashs_de_varios_inputs_semelhantes(void)
{
   size_t cp = 20;
   wchar_t* amostras[] ={
      // Maiúsculas e minúsculas, não nesta ordem:
      L"a", L"b", L"c", L"d",
      L"A", L"B", L"C", L"D",
      // Frase com pequenas variações.
      L"Casinha do seu Zé",
      L"casinha do seu zé",
      // Mesma letras, porém alternadas:
      L"relógio", L"rElÓgIo",
      // Mesma quantia de letras, sem acentos:
      L"laranja", L"pessego", L"abacaxi", L"morango"
   };

   puts("Chaves e determinados hash, observe as proximidades:");
   for (int i = 1; i <= 16; i++)
   {
      wchar_t* key = amostras[i - 1];
      printf("\t%ls ===> %zu\n", key, registro_hash(key, cp));
   }

   puts("Outro método hash aqui, menos veloz, no entanto, mais dispersão:");
   for (int i = 1; i <= 16; i++)
   {
      wchar_t* key = amostras[i - 1];
      printf("\t%ls ===> %zu\n", key, registro_hash_i(key, cp));
   }
}

void hash_de_frases_com_pouca_variacao(void)
{
   size_t cp = 100;
   wchar_t* amostras[] = {
      L"I look to you and i see nothmng",
      L"i looK to you and i see nothING",
      L"I look to you and j see nothang",
      L"I look to you aNd a see nothbng",
      L"I look to you and i see nothicg",
      L"I look to you #nd # see noteing",
      L"I look to you an. i see not_ing",
      L"I look go yol aNd a see nothbng",
      L"I look ti yol and i see nothicg",
      L"I look tk yol #nd # see noteing",
      L"I look to you and i sei not_ing"
   };

   puts("Chaves e determinados hash, observe as proximidades:");
   for (int i = 1; i <= 10; i++)
   {
      wchar_t* key = amostras[i - 1];
      printf("\t'%ls' ===> %zu\n", key, registro_hash(key, cp));
   }
   puts("Outro método hash aqui, menos veloz, no entanto, mais dispersão:");
   for (int i = 1; i <= 10; i++)
   {
      wchar_t* key = amostras[i - 1];
      printf("\t'%ls' ===> %zu\n", key, registro_hash_i(key, cp));
   }
}

void igualdade_entre_as_chaves(void)
{
   wchar_t* inputs[] = {
      L"J", L"j", L"a", L"B", L"e", L"e", L"m", L"n", L"z", L"a",
      L"casinha", L"cazinha", L"portAA", L"portaA", L"pineapple",
      L"pineapple", L"Thousand of Years", L"Thousand of Years",
      L"editor", L"iditor"
   };

   bool outputs[] = {
      false, false, true, false, false, false, false, true, true,
      false
   };

   puts("Verificando os pares...");
   for (int i = 0; i < 2*10; i+=2)
      printf("\t\b\b\b>>> %ls = %ls?\n", inputs[i + 1], inputs[i]);

   printf("\nSão todas as saídas correspodentes?");
   fflush(stdout);

   for (int i = 0, j = 0; i < (2 * 10); i += 2, j++)
   {
      bool comparacao =  registro_eq(inputs[i + 1], inputs[i]);
      bool saida = outputs[j];
      assert(saida == comparacao);
   }
   puts(" ... sim.");
}

int main(void) {
   // Ativando uso de estringues Unicode...
   setlocale(LC_CTYPE, "");
   
   executa_testes(
      3, visualiza_hashs_de_varios_inputs_semelhantes, true,
         hash_de_frases_com_pouca_variacao, true,
         igualdade_entre_as_chaves, true
   );

   return EXIT_SUCCESS;
}
#endif
