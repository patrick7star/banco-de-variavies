
#ifndef _BANCO_H
#define _BANCO_H
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

 // Padrão dos caminhos abaixos:
 #define N 500

 // Nome de estruturas internas:
 typedef struct metadados_de_um_banco metadados_t, MetaDados;
 typedef struct total_de_operacoes_por_uso_do_banco SaldoOperacoes;
 typedef struct historico_dele_durante_o_uso Historico;
 typedef struct banco_de_dados Banco;

 // Definição de estruturas:
 struct PathArqs { 
 /* Par de caminhos referentes aos bancos criados. Muitas funções abaixos,
  * geral tal estrutura. O 'data' refere-se ao caminho que localiza o 
  * arquivo de dados binários, já o 'mutex' é sobre quem pode escrever 
  * no banco no momento(parecido com o mutex de threads). */
    // Caminho para o arquivo com dados armazenados.
    char data[N]; 

    // Caminho para o guardador de utilizadores.
    char mutex[N]; 

    // Caminho para metadados do BD.
    char metadata[N];

    // Histórico deste BD ao longo de vários usos.
    char history[N];
 };

 // Todos métodos das definições acimas:
 struct Bytes metadados_serializacao(MetaDados* ptr);
 struct PathArqs gera_caminhos(char* dir, size_t id);
 bool cria_banco_personalizado(char* diretorio, size_t id);
 bool cria_banco_rapido(void);

#endif

#ifndef _BANCO_PUBLICO_H
#define _BANCO_PUBLICO_H
#include <time.h>
/* Implementação das estruturas pois algumas requerem 'campos públicos'. 
 * Está tudo sendo posto aqui embaixo, porque é um detalhe na verdade, não
 * é tão importante assim.
 */

struct metadados_de_um_banco
{
   /* Quando foi criado, e última vez que foi alterado: Estes são melhores
    * que o que os atuais arquivos guardam, porque são resistentes a 
    * alterações. */
   time_t criacao; time_t alteracao;

   /* Total de bytes que armazena. Difere de apenas verificar o tamanho
    * do arquivo, porque este desconta os bytes que dizem o pŕoximo tanto
    * de bytes a ler. */
   size_t tamanho_em_bytes;

   // Quantidade total de variáveis que o atual BD guarda.
   size_t qtd_de_variaveis;
};

struct total_de_operacoes_por_uso_do_banco
{
/* Todas operações realizadas no 'banco de dados'. Os nomes de cada campo
 * já dizem por si só prá que servem cada um. */
   float insercoes;
   float remocoes;
   float atualizacoes;
   float consultas;
};

// Tupla para agregar dados para o tipo que armazena arrays disso abaixo.
struct TuplaRegistro { 
   MetaDados      dados; 
   SaldoOperacoes agregado; 
   time_t         tempo; 
};

struct historico_dele_durante_o_uso
{
/* A cada uso(fechamento da estrutura), o seu saldo de realizações é gravado
 * para uso estatístico posteriormente. Ele guarda o pico de fluxo das 
 * operações sobre o banco, e também o estado final após o encerramento.
 * Todas arrays contém o mesmo comprimento. 
 */
   // Sequencia com os três dados a serem gravados.
   struct TuplaRegistro* array;
   // Quantidade total de entradas da array acima.
   size_t total;
};

#endif
