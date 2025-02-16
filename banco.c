
// Declaração das estruturas e métodos abaixos:
#include "banco.h"
// Bibliotecas externas:
#include "hashtable_ref.h"
#include "aleatorio.h"
#include "teste.h"
// Biblioteca POSIXs:
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
// Biblioteca padrão do C:
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


// Constantes importantes...
#define N 500 // Comprimento para todos caminhos.
// Confirmação ou negação de chamadas do sistema.
const int Failed = - 1, Okay = 0;
// Nome dos arquivos.
const char* BD_NOME  = "banco";
const char* MTX_NOME = "cadeado";
const char* MTD_NOME = "metadados";
const char* HT_NOME  = "historico";
// Bytes mágicos de identificação do arquivo:
const uint8_t BANCO_ID[]    = {0x42, 0x41, 0x4e, 0x43, 0x4f, 0x0};
const uint8_t* HISTORICO_ID = (unsigned char*)"HISTORICO";


struct banco_de_dados
{
   /*   Diretório onde os arquivos serão postos, e o ID para identificar 
    * este único banco lá, seja o binário, ou cadeado para excluir múltiplos
    * escritores nele.
    *
    *   Seus metadados: taxa de consulta, taxa de alteração, última 
    * alteração, criação, tamanho(em bytes), e etc... 
    *
    *   Quantas operações(de cada tipo) foram feita durante uso. */
   char* raiz; size_t id;
   MetaDados metadata;
   SaldoOperacoes saldo;

   /* Tabela que adiciona, remove, consulta e atualiza o banco, que foi 
    * carregado na memória, que no fim será despejado no 'mass storage'. */
   HashTable tabela;

   /* Histórico do 'saldo de operações' e 'metadados' do banco durante
    * o tempo. */
   Historico serie_historica;

   // Streams referentes aos dados e ao uso dele(mutex).
   FILE* conteudo; FILE* cadeado;
};

struct PathArqs gera_caminhos(char* dir, size_t id)
{
/* Gera caminho, baseado no ID dado, de todos arquivos referentes a um
 * banco criado. */
   struct PathArqs result;

   /* Nomeando o arquivo que guarda os binários, e o cadeado que permite
    * este banco ser multithreading, ou mesmo multiprocesso. */
   sprintf(result.data, "%s/%s.%zu.db", dir, BD_NOME, id);
   sprintf(result.mutex, "%s/%s.%zu.dat", dir, MTX_NOME, id);
   sprintf(result.metadata, "%s/%s.%zu.dat", dir, MTD_NOME, id);
   sprintf(result.history, "%s/%s.%zu.dat", dir, HT_NOME, id);

   return result;
}

bool cria_banco_personalizado(char* diretorio, size_t id)
{
/* Cria um banco no caminho dado, com determinado ID. Se já existe um lá
 * com este, a operação será interrompida, portanto o programa parará. 
 * Os arquivos referentes serão criado em tal diretório, se ele existir,
 * caso não, então serão todos criados, mesmo que a operação aponte um 
 * frasasso. */
   struct PathArqs path = gera_caminhos(diretorio, id);
   bool em_uso = false;
   FILE* arq;
   struct stat a, b, c;

   // Referente a raíz.
   stat(diretorio, &a);
   bool nao_diretorio = (S_ISDIR(a.st_mode) == 0);

   int result_d = stat(path.data, &b);
   int result_e = stat(path.mutex, &c); 
   bool arquivos_ja_existentes = {
      result_e != Failed || 
      result_d != Failed 
   };

   if (arquivos_ja_existentes || nao_diretorio)
   /* A 'raíz' passada tem que ser um diretório, e os caminhos dados
    * não pode já existir, qualquer um deles. */
      return false;

   // Gerando arquivo com os dados da 'tabela'...
   arq = fopen(path.data, "wb");
   int result_a = fclose(arq) == Okay;

   // Gerando o arquivo que permite apenas um operador por vez.
   arq = fopen(path.mutex, "wb");
   int result_c = fputc(em_uso, arq) != EOF;
   int result_b = fclose(arq) == Okay;

   // Gerando arquivo com metadados do banco.
   arq = fopen(path.metadata, "wb");
   /* Escrevendo os 'metadados', seguindo a ordem que estão escritos na 
    * estrutura abstrata. */
   fclose(arq);

   // Gerando o arquivo com histórico do BD.
   arq = fopen(path.history, "wb");
   fclose(arq);

   return result_a && result_b && result_c;
}

bool cria_banco_rapido(void)
{
/* Cria um banco, como acima, porém no atual diretório, e com um ID que foi
 * selecionado de forma randômica. */
   size_t ID = inteiro_positivo(1, 1000);
   char dir[N];

   // Pega atual diretório.
   if (getcwd(dir, N) == NULL) 
      return false;
   // Cria o banco, chamando a função personalizada acima.
   return cria_banco_personalizado(dir, ID);
}

static bool arquivo_existe(char* caminho)
{
   struct stat info;
   int code = stat(caminho, &info);

   if (code == Okay)
      return false;
   else {
      if(errno == ENOENT)
         return false;
      else {
         perror(strerror(errno));
         abort();
      }
   }
   /* Se tudo ocorreu corretamente, então apenas confirma a existência. */
   return true;
}

Banco abre_banco(char* diretorio, size_t id)
{
   struct PathArqs caminhos = gera_caminhos(diretorio, id);
   Banco obj;

   // Verifica se cada arquivo existe. Se não existirem, apenas cria todos
   // eles.
   bool tem_todos_requisitos = {
      arquivo_existe(caminhos.data) &&
      arquivo_existe(caminhos.mutex) &&
      arquivo_existe(caminhos.metadata) &&
      arquivo_existe(caminhos.history)
   };

   #ifdef __debug__
   printf(
      "Todos arquivos são existentes? %s\n", 
      bool_to_str(tem_todos_requisitos)
   );
   #endif

   if (!tem_todos_requisitos)
   {
      cria_banco_personalizado(diretorio, id);

      obj.raiz = diretorio;
      obj.id = id;
      obj.conteudo = fopen(caminhos.data, "wr+b");
      obj.cadeado = fopen(caminhos.mutex, "wr+b");
      obj.tabela = NULL;

      /* Diz que está em uso no momento . Lembrando que o valor falso é 0x1,
       * que em decimal é 1, e valor verdadeiro é 0xff(em decimal é 255).
       * Qualquer outra coisa além disso é ruído, portanto indica que tal
       * arquivo está danificado. */
      fputc(0xff, obj.cadeado);
      // O número mágico do 'banco de dados'.
      fputs((char*)BANCO_ID, obj.conteudo);
   }
}

void fecha_banco(Banco a) { }


#ifdef __unit_tests__
/* === === === === === === === === === === === === === === === === === ==
 *                            Testes Unitários
 *    Faz uma série de testes dos dados estruturados, e as funções que o
 * seguem(métodos) acima.
 * === === === === === === === === === === === === === === === === === ==*/
#include <assert.h>
#include <locale.h>
#include "teste.h"

void criacao_dos_arquivos_necessarios(void)
{
   int IDs[] = {3, 13, 666, 1234, 1024, 72}, id;
   char* dir = "/tmp";
   struct PathArqs path;

   for (int i = 0; i < 6; i++)
   {
      id = IDs[i];
      assert(cria_banco_personalizado(dir, id));

      putchar('\n');
      path = gera_caminhos(dir, id);
      printf("Excluindo '%s'...\n", path.data);
      assert(remove(path.data) == Okay);
      printf("Excluindo '%s'...\n", path.mutex);
      assert(remove(path.mutex) == Okay);
      printf("Excluindo '%s'...\n", path.history);
      assert(remove(path.history) == Okay);
      printf("Excluindo '%s'...\n", path.metadata);
      assert(remove(path.metadata) == Okay);
   }
}

void criacao_simples_no_atual_diretorio(void)
{
   char dir[N];
   assert(getcwd(dir, N) != NULL);
   printf("Atual diretório: '%s'\n", dir);

   for (int i = 1; i <= 10; i++) 
   { 
      bool result = cria_banco_rapido(); 
      const char* boolstr = bool_to_str(result);
      printf("Resultado da criação? %s\n", boolstr);
   }

   // Listagem dos arquivos atualmente...
   DIR* listagem = opendir(dir);
   char path_remove[200];

   puts("Atualmente no diretório; excluindo arquivos gerados:");
   do {
      struct dirent* entry = readdir(listagem);

      // Sai quando acabar as entradas.
      if (entry == NULL) break;

      char* name = entry->d_name;
      bool e_para_excluir = {
         // Sobre o primeiro tipo de arquivo...
         (
            strstr(name, "cadeado.") != NULL && 
            strstr(name, ".dat") != NULL
         ) || (
         // Sobre o segundo tipo de arquivo...
            strstr(name, ".db") != NULL && 
            strstr(name, "banco.") != NULL
         ) || (
         // Terceiro arquivo...
            strstr(name, ".dat") != NULL && 
            strstr(name, "metadados.") != NULL
         ) || (
         // Terceiro arquivo...
            strstr(name, ".dat") != NULL && 
            strstr(name, "historico.") != NULL
         )
      };

      if (e_para_excluir) {

         sprintf(path_remove, "%s/%s", dir, name);
         printf("\t- '%s'(excluído)\n", name);
         remove(path_remove);

      } else
         printf("\t- '%s'\n", name);
   } while(true);
}

void invalidade_de_abrir_banco_inexistente(void)
{
   Banco obj = abre_banco("./data/tests", 12);
}

int main(void)
{
   // Ativando uso de estringues Unicode...
   setlocale(LC_CTYPE, "");
   
   executa_testes_a(
      ture, 3, 
         criacao_dos_arquivos_necessarios, true,
         criacao_simples_no_atual_diretorio, true,
         invalidade_de_abrir_banco_inexistente, true
   );

   return EXIT_SUCCESS;
}
#endif
