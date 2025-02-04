/*   Banco simples que armazena algunas variáveis básicas para qualquer 
 * aplicação que desejar. Os tipos que eles armazena são quase todos 
 * primitivos, mais contando também uma string, que uma simples array do
 * tipo 'caractére', entretanto em Unicode. 
 *   
 *   Eu tentei fazer um deste antes, ficou quase pronto, porém era isso aí,
 * sempre ficava quase pronto, mais nunca saia do alfa para uso, enquanto
 * o projeto aumentava de tamanho e complexidade. Com esta nova 
 * implmenetação será diferente, sairá uma versão final rápida, colocarei 
 * até no GitHub para mostrar compromisso com o projeto. Depois que vou 
 * aperfeiçoando ele e tal.
 */
// Biblioteca padrão do C:
#include <locale.h>
// Módulos referentes a este pacote:
#include "dados.h"
#include "tabela.h"
#include "banco.h"


#ifdef __unit_tests__
int main(void)
{
   // Ativando uso de estringues Unicode...
   setlocale(LC_CTYPE, "");
   
   #elif defined(__ut_tabela__)
   // Testes referentes ao "módulo" banco.c.
   executa_testes(
      3, visualiza_hashs_de_varios_inputs_semelhantes, true,
         hash_de_frases_com_pouca_variacao, true,
         igualdade_entre_as_chaves, true
   );

   #endif

   return EXIT_SUCCESS;
}
#else
int main(int quantidade, char* args[], char* envs[])
{
   setlocale(LC_CTYPE, "");
   return EXIT_SUCCESS;
}
#endif
