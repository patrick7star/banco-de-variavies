/*   Serialização/ e deserilização de vários tipos primitivos, ou estruturas
 * um pouco mais complexas, necessárias para este programa. 
 *   A ordem de arranjo dos bytes de cada campo de variáveis segue, 
 * estritamente, a ordem com que está declarada na implementação deste tipo
 * de estrutura abstrata. 
 */

// Declaração dos tipos de dados, funções e métodos abaixo:
#include "serializacao.h"
// Biblioteca padrão do C:
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <wchar.h>
// Biblioteca externa:
#include "conversao.h"
#include "dados.h"
#include "hashtable_ref.h"

const char* METADADOS_ID = "metadados";
const char* CADEADO_ID = "controle";
const char* TABELA_ID = "conteudo";


static uint8_t* serializa_metadados(MetaDados* in)
{
   int size = sizeof(MetaDados);
   uint8_t* out = malloc(size);

   /* Apenas copia todos bytes do objeto passado por parâmetro para array
    * de bytes. O tamanho da array é devidamente alocado baseado no tipo
    * da variável. Por fim, apenas retorna os bytes copiados. */
   memcpy(out, in, size);
   return out;
}

static MetaDados deserializa_metadados(uint8_t* in)
{
   MetaDados out; 
   /* Quando penso em tamanho em bytes, não penso em unidades, e sim em 
    * algo homogêneo, porém infinito. Apenas na parte de copiar que me 
    * vem a cabeça unidades. Por isso os nomes diferentes para a mesma
    * coisa. */
   int size = sizeof(MetaDados);
   int total = size;

   /* Copia os bytes passados para o endereço do tipo que será retornado,
    * basicamente uma deserialização crua, mas automática.*/
   memcpy(&out, in, total);
   return out;
}

static uint8_t* serializa_saldo(SaldoOperacoes* in)
{
   int size = sizeof(SaldoOperacoes);
   uint8_t* out = malloc(size);

   memcpy(out, in, size);
   return out;
}

static SaldoOperacoes deserializa_saldo(uint8_t* in)
{
   SaldoOperacoes output; 
   int total = sizeof(SaldoOperacoes);

   memcpy(&output, in, total);
   return output;
}

static struct TuplaRegistro deserializa_tr(uint8_t* in)
{
   struct TuplaRegistro saida;
   int sz = sizeof(struct TuplaRegistro);

   memcpy(&saida, in, sz);
   return saida;
}

struct Bytes serializa_historico(Historico* in)
{
/*   Como estamos trabalhando aqui com uma estringue de array 'variável', o 
 * retorno será numa 'estrutura de bytes', que é uma tupla com o tamanho
 * da array de bytes trago consigo. 
 *   A ordem de seriazação segue a codificação, com uma exceção, a série de
 * bytes dinâmicos sempre vem depois do tamanho conhecido em tempo de 
 * compilação. */
   struct Bytes output; 
   // Quantidade de itesn e o endereço da array de tuplas.
   size_t n = in->total;
   struct TuplaRegistro* array = in->array;
   // Total de bytes dos tipos 'inteiro positivo de máquina' e 'tupla'.
   size_t a = sizeof(struct TuplaRegistro);
   size_t b = sizeof(size_t);
   // Só para diferênciar na leitura, técnica ajuda na legibilidade.
   size_t quantia = n;
   uint8_t* bytes;

   output.bytes = malloc(n * a + b);
   output.total = n * a + b;

   bytes = output.bytes;
   memcpy(bytes, &quantia, b);
   bytes = output.bytes + b;
   memcpy(bytes, array, n * a);

   #ifdef __debug__
   puts("Todos bytes a serem deserializados:");
   print_array(output.bytes, output.total, true);
   #endif

   return output;
}

Historico deserializa_historico(struct Bytes* in)
{
/* Os primeiros oito bytes contém um valor do tipo inteiro máximo sem sinal.
 * Os restantes bytes são uma array do tipo TuplaRegistro. 
 *
 * Nota: Array interna aloca memória na 'heap', logo precisa ser desalocada. */
   Historico objeto; 
   // Tanto de bytes em cada tipo codificado.
   const int a = sizeof(size_t);
   const int b = sizeof(struct TuplaRegistro);
   // Decodificando o comprimento da array...
   size_t t = from_bytes_to_sizet(in->bytes);
   struct TuplaRegistro* array = malloc(t * b);

   for (int i = 0; i < t; i++)
   {
      /* Calculando a posição correta na array de bytes equivalente à
       * próxima tupla de registro que será agregada. */
      uint8_t* bytes = in->bytes + a + b * i; 
      array[i] = deserializa_tr(bytes);
   }
   /* A array alocada é "passada" para o campo do objeto a ser retornado. 
    * Seu comprimento também é copiado da variável local que decodifica ele
    * da estringue de bytes.*/
   objeto.array = array;
   objeto.total = t;

   return objeto;
}

struct Bytes serializa_registro(Registro* e)
{
/*   Seguindo o padrão das demais serialização, aqui será feito o seguinte:
 * primeiro, a parte de dados estáticos, eles não variam seu tamanho, que 
 * neste caso aqui, são todos, menos a estringue. Segundo a parte dinâmica,
 * ou seja, a estringue com seu comprimento na frente, aliás, como irei ler
 * o total de caractéres, se ainda não sei o total, certo. 
 *
 *   Isso fica como uma exceção no caso aqui, pois a estringue -- que não 
 * tem tamanho em tempo de compilação -- é o primeiro campo, seguindo tal 
 * exceção, ela ficaria em último, tirando a regra geral de seguir a ordem 
 * que está na codificação do tipo de dado. Então, a ordem de codificação 
 * em bytes ficou desta maneira: valor(struct), tipo(enum), criação(time_t),
 * atualização(time_t), e aí sim a chave(struct). */
   struct Bytes output; 
   // Quantidade de bytes de cada tipo primitiva utilizado abaixo:
   const int a = sizeof(union Value), b = sizeof(time_t), 
     c = sizeof(DadoPropriedade), d = sizeof(wchar_t), f = sizeof(size_t);
   size_t n = (*e).chave.len;
   // Somando na ordem que são serializados e concatenados. 
   int size = a + c + 2*b + f + n*d;

   #ifdef __debug__
   printf("'union Value' total de bytes: %d\n", a);
   printf("DadoPropriedade total de bytes: %d\n", c);
   printf("wchar_t total de bytes: %d\n", d);
   #endif

   output.bytes = malloc(size);
   output.total = size;
   memset(output.bytes, 0xff, output.total);
   uint8_t* ptr = output.bytes;

   memcpy(ptr, &((*e).valor), a);
   // Move o tanto de bytes copiado.
   ptr += a;
   memcpy(ptr, &((*e).tipo), c);
   ptr += c;
   memcpy(ptr, &((*e).criacao), b);
   ptr += b;
   memcpy(ptr, &((*e).atualizacao), b);
   ptr += b;

   /* Agora a parte que pode variar de tamanho. E como dito, primeiro vem
    * o total de bytes, e então seus bytes. Não será preciso mover o 
    * cursor(pointeiro) o total de bytes alocado, pois é o último campo
    * até o momento. */
   // Endereço de ambos valores a começar copiar seus bytes.
   wchar_t* bytes_str = (*e).chave.array;
   size_t* bytes_len = &n;

   memcpy(ptr, bytes_len, f); 
   ptr += f;
   // memcpy(ptr, e->chave.array, e->chave.len * sizeof(wchar_t));
   memcpy(ptr, bytes_str, n * d);

   return output;
}

Registro deserializa_registro(struct Bytes* seq)
{
/* Deserialização do tipo 'Registro' que é o valor que qualquer chave na 
 * 'tabela de dispersão' irá portar. */
   // Obtendo o tamanho de cada tipo utilizado no processo abaixo:
   const int a = sizeof(union Value), b = sizeof(DadoPropriedade),
     c = sizeof(time_t), d = sizeof(size_t), e = sizeof(wchar_t);
   uint8_t* ptr = seq->bytes;
   union Value valor;  
   DadoPropriedade tipo;
   time_t criacao, atualizacao;
   size_t comprimento;
   wchar_t* str;

   memcpy(&valor, ptr, a); 
   ptr += a;
   memcpy(&tipo, ptr, b);
   ptr += b;

   memcpy(&criacao, ptr, c);
   ptr += c;
   memcpy(&atualizacao, ptr, c);
   ptr += c;

   /* Agora a parte dinâmica, a estringue: primeiro o total de caractéres
    * dela, então os bytes dela. Lembre-se: na hora de alocar a memória da 
    * estringue multiplica por 4(ou o tanto de bytes do tipo 'wchar_t') 
    * porque ela é uma estringue Unicode. */
   memcpy(&comprimento, ptr, d);
   ptr += d;
   str = malloc(comprimento * e);
   memcpy(str, ptr, comprimento * e);

   #ifdef __debug__
   const int linha = __LINE__;
   const char* arq = __FILE__;
   wchar_t* fmt_valor = value_to_str(tipo, &valor);

   printf("\n[%s | %d]chave: '%ls'[%zu]\n",  arq, linha, str, comprimento);
   printf("[%s | %d]valor: %ls\n",  arq, linha, fmt_valor);
   printf("[%s | %d]tipo: %s\n",  arq, linha, dp_to_str(tipo));
   printf("[%s | %d]criação: %lu\n",  arq, linha, criacao);
   printf("[%s | %d]atualização: %lu\n",  arq, linha, atualizacao);
   free(fmt_valor);
   #endif

   return (Registro) {
      .chave       = {.array = str, .len=comprimento},
      .valor       = valor,
      .tipo        = tipo,
      .criacao     = criacao,
      .atualizacao = atualizacao
   };
}

struct Bytes serializa_tabela(HashTable input)
{
/*   A tabela aqui trabalhada é do tipo 'wchar_t' como chave, e 'Registro'
 * como valor. Por o tamanho não ser conhecido em tempo de compilação os
 * campos estáticos virão primeiro, na ordem de codificação, depois a 
 * parte dinâmica. */
   const int n = tamanho_ht(input);
   IterHT iteracao = cria_iter_ht(input);
   struct Bytes fila_keys[n], fila_values[n];
   size_t size = 0, p = 0;
   struct Bytes saida;

   while(!consumido_iter_ht(iteracao))
   {
      IterOutputHT e = next_ht(iteracao);

      /* Tanto serializa a chave e o valor, como também ao mesmo tempo
       * computa quanto cada um ocupa de tamanho em bytes. */
      if (e.key != NULO_HT.key && e.value != NULO_HT.value)
      {
         wchar_t* key = (wchar_t*)e.key;
         Registro* value = (Registro*)e.value;

         fila_keys[p] = string_unicode_to_bytes(key); 
         size += fila_keys[p].total;
         fila_values[p] = serializa_registro(value);
         size += fila_values[p].total;
         p++;

         #ifdef __debug__
         int q = p - 1;
         print_array(fila_keys[q].bytes, fila_keys[q].total, true);
         print_array(fila_values[q].bytes, fila_values[q].total, true);
         printf("Tamanho de cada: %lu.\n", wcslen(key));
         #endif
      }
   }
   // saida.bytes = malloc(size * sizeof(uint8_t));
   saida.total = size;
   /* Agora com a quantia de bytes necessária em mãos, aloca-se a array 
    * de bytes a copiar todoas estas outras. */
   saida.bytes = malloc(size);

   uint8_t* dst, *src;
   const int sz = sizeof(size_t);
   uint8_t buffer [sz];

   /* Retirando tudo da fila e concatenando na array que conglomera todos
    * eles. */
   for (size_t i = 0, m = 0; i < p; i++)
   {
      /* Primeiro serão os bytes da chave. Como ela tem um tamanho, este 
       * será serializado e colocado na frente dos bytes da chave 
       * propriamente dita, ou seja, a chave, porém existe uma quantia de 
       * bytes desta array de bytes referente a ela, serializamos tal, e 
       * colocamos na frente dela, muito por ter um tamanho estático que 
       * vai na frente, seguindo a regra de outros tipos de serializaçao 
       * realizados. 
       */
      m = fila_keys[i].total;
      sizet_to_bytes(m, buffer);
      dst = saida.bytes;
      src = buffer;
      memcpy(dst, src, sz);

      dst = (saida.bytes + sz);
      src = fila_keys[i].bytes;
      memcpy(dst, src, m);
      
      /* Segundo, os bytes do valor. Pelo mesmo motivo da acima: a quantia
       * de bytes tem um valor, este valor é fixo, portanto vai primeiro. */
      m = fila_values[i].total;
      sizet_to_bytes(m, buffer);
      dst = saida.bytes;
      src = buffer;
      memcpy(dst, src, sz);

      dst = (saida.bytes + sz);
      src = fila_values[i].bytes;
      memcpy(dst, src, m);
   }

   /* Como tais bytes já foram copiados, vamos então fazer o processo de 
    * liberação das serializações feitas. */
   for (size_t i = 0; i < p; i++) {
      free_bytes(&fila_keys[i]);
      free_bytes(&fila_values[i]);
   }

   return saida;
}

// Tupla como os principais valores do retorno da função abaixo.
struct TuplaRegistroII { wchar_t* chave; Registro valor; };

static struct TuplaRegistroII 
extrai_entrada_de_registro(uint8_t* input, size_t* lido)
{
/*  Não foi feita uma copia dos */
   struct TuplaRegistroII output;
   struct Bytes K, V;
   size_t ta, tb, sz = sizeof(size_t);

   /* Transformando um trecho em duas 'struct Bytes' de ambos valores da 
    * tupla que é retornada. */
   ta = from_bytes_to_sizet(input);
   K.bytes = input + sz;
   K.total = ta;
   tb = from_bytes_to_sizet(input + sz + ta);
   V.bytes = input + sz + ta + sz;
   V.total = tb;

   #ifdef __debug__
   puts("\nBytes da 'chave' e 'valor' extraídos, respectivamente:");
   print_array(K.bytes, K.total, true);
   print_array(V.bytes, V.total, true);
   #endif

   // output.chave = from_bytes_to_string_unicode(&K);
   output.valor = deserializa_registro(&V);

   #ifdef __debug__
   //printf("chave: '%ls'\n", output.chave);
   debug_registro(&output.valor);
   #endif

   // Registrando quanto foi lido de bytes.
   *lido += sz + ta + sz + tb; 
   return output;
}

#include "tabela.h"
HashTable deserializa_tabela(struct Bytes* input)
{
/*   O algoritmo de serializa coloca a série de bytes de cada entrada da 
 * tabela(chave string e Registro valor) na ordem que a iteração do mesmo
 * cospe eles. A serialização de tal 'entrada' é a série de bytes do 
 * comprimento da string que representa a chave, então os bytes da string
 * em sí. O mesmo vale para o registro, seu tamanho codificado em bytes,
 * daí seus bytes. */
   HashTable output = cria_ht(registro_hash, registro_eq);
   size_t lido = 0;
   struct TuplaRegistroII e;
   uint8_t* bytes = input->bytes;

   while (lido < input->total)
   {
      e = extrai_entrada_de_registro(bytes, &lido);
      assert(insere_ht(output, &e.chave, &e.valor));
      getchar();
   }

   return output;
}

#ifdef __unit_tests__
/* === === === === === === === === === === === === === === === === === ==
 *                            Testes Unitários
 * === === === === === === === === === === === === === === === === === ==*/
#include <locale.h>
#include "teste.h"
#include "aleatorio.h"

static void debug_metadados(MetaDados* obj)
{
   printf(
      "Metadados {\n\t\b\b\b\tcriação: %lu,\n\t\b\b\balteração: %lu, "
      "\n\t\b\b\btamanho: %lu,\n\t\b\b\btotal de variáveis: %lu\n}\n", 
      obj->criacao, obj->alteracao, obj->tamanho_em_bytes, 
      obj->qtd_de_variaveis
   );
}

static void debug_saldo(SaldoOperacoes* obj)
{
   printf(
    "Saldo de Operações {\n\t\b\b\binserções: %0.2f,\n\t\b\b\bremoções: "
    "%0.2f, \n\t\b\b\batualizações: %0.2f,\n\t\b\b\bconsultas: %0.2f\n}\n", 
      obj->insercoes, obj->remocoes, obj->atualizacoes, obj->consultas
   );
}

static void debug_tuplaregistro(struct TuplaRegistro* obj)
{
   printf(
    "Tupla Registro {\n\t- dados(MetaDados) {\n\t\tcriação: %lu"
    "\n\t\talteração: %lu\n\t\ttamanho: %lu\n\t\tqtd de variáveis:%lu\n\t}"
    "\n\t- agregado(SaldoOperacoes) {\n\t\tinserções: %0.2f\n\t\tremoções "
    "%0.2f\n\t\tatualizações: %0.2f\n\t\tconsultas: %0.2f\n\t}"
    "\n\t- tempo(time_t) { %luseg }\n}\n",
      obj->dados.criacao, obj->dados.alteracao, obj->dados.tamanho_em_bytes,
      obj->dados.qtd_de_variaveis, obj->agregado.insercoes, 
      obj->agregado.remocoes, obj->agregado.atualizacoes, 
      obj->agregado.consultas, obj->tempo
   );
}

static void debug_historico(Historico* obj)
{
   assert(obj->total >= 4);

   // Dois objetos no meio da sequência, e as pontas.
   struct TuplaRegistro t1, ti, tj, tn;
   int m = obj->total / 2, f = obj->total - 1;
   
   t1 = obj->array[0];
   ti = obj->array[m];
   tj = obj->array[m + 1];
   tn = obj->array[f];

   printf("Historico(%lu) {\n", obj->total);
   printf(
    "  Tupla Registro {\n\t- dados(MetaDados) {\n\t\tcriação: %lu"
    "\n\t\talteração: %lu\n\t\ttamanho: %lu\n\t\tqtd de variáveis:%lu\n\t}"
    "\n\t- agregado(SaldoOperacoes) {\n\t\tinserções: %0.2f\n\t\tremoções "
    "%0.2f\n\t\tatualizações: %0.2f\n\t\tconsultas: %0.2f\n\t}"
    "\n\t- tempo(time_t) { %luseg }\n  }\n",
      t1.dados.criacao, t1.dados.alteracao, t1.dados.tamanho_em_bytes,
      t1.dados.qtd_de_variaveis, t1.agregado.insercoes, 
      t1.agregado.remocoes, t1.agregado.atualizacoes, 
      t1.agregado.consultas, t1.tempo
   );
   printf(
    "\n  Tupla Registro {\n\t- dados(MetaDados) {\n\t\tcriação: %lu"
    "\n\t\talteração: %lu\n\t\ttamanho: %lu\n\t\tqtd de variáveis:%lu\n\t}"
    "\n\t- agregado(SaldoOperacoes) {\n\t\tinserções: %0.2f\n\t\tremoções "
    "%0.2f\n\t\tatualizações: %0.2f\n\t\tconsultas: %0.2f\n\t}"
    "\n\t- tempo(time_t) { %luseg }\n  }\n",
      ti.dados.criacao, ti.dados.alteracao, ti.dados.tamanho_em_bytes,
      ti.dados.qtd_de_variaveis, ti.agregado.insercoes, 
      ti.agregado.remocoes, ti.agregado.atualizacoes, 
      ti.agregado.consultas, ti.tempo
   );
   printf(
    "\n  Tupla Registro {\n\t- dados(MetaDados) {\n\t\tcriação: %lu"
    "\n\t\talteração: %lu\n\t\ttamanho: %lu\n\t\tqtd de variáveis:%lu\n\t}"
    "\n\t- agregado(SaldoOperacoes) {\n\t\tinserções: %0.2f\n\t\tremoções "
    "%0.2f\n\t\tatualizações: %0.2f\n\t\tconsultas: %0.2f\n\t}"
    "\n\t- tempo(time_t) { %luseg }\n  }\n",
      tj.dados.criacao, tj.dados.alteracao, tj.dados.tamanho_em_bytes,
      tj.dados.qtd_de_variaveis, tj.agregado.insercoes, 
      tj.agregado.remocoes, tj.agregado.atualizacoes, 
      tj.agregado.consultas, tj.tempo
   );
   printf(
    "\n  Tupla Registro {\n\t- dados(MetaDados) {\n\t\tcriação: %lu"
    "\n\t\talteração: %lu\n\t\ttamanho: %lu\n\t\tqtd de variáveis:%lu\n\t}"
    "\n\t- agregado(SaldoOperacoes) {\n\t\tinserções: %0.2f\n\t\tremoções "
    "%0.2f\n\t\tatualizações: %0.2f\n\t\tconsultas: %0.2f\n\t}"
    "\n\t- tempo(time_t) { %luseg }\n  }\n",
      tn.dados.criacao, tn.dados.alteracao, tn.dados.tamanho_em_bytes,
      tn.dados.qtd_de_variaveis, tn.agregado.insercoes, 
      tn.agregado.remocoes, tn.agregado.atualizacoes, 
      tn.agregado.consultas, tn.tempo
   );
   puts("}");
}

void serializacao_do_tipo_metadados(void)
{
   MetaDados input = {
      .criacao          = 70,
      .alteracao        = 100,
      .tamanho_em_bytes = 2000,
      .qtd_de_variaveis = 36
   }, output;
   uint8_t* bytes = serializa_metadados(&input);

   debug_metadados(&input);
   output = deserializa_metadados(bytes); 
   debug_metadados(&output);

   free(bytes);
}

void processo_de_serializacao_do_saldo(void)
{
   SaldoOperacoes a, A = {
      .insercoes    = 30.5,
      .remocoes     = 10.8,
      .atualizacoes = 37.9,
      .consultas    = 1.53
   };
   uint8_t* a_bytes;

   debug_saldo(&A);
   a_bytes = serializa_saldo(&A);
   a = deserializa_saldo(a_bytes); 
   debug_saldo(&a);
   free(a_bytes);
}

static uint8_t* serializa_tr(struct TuplaRegistro* in)
{
   int total = sizeof(struct TuplaRegistro);
   int size = total;
   uint8_t* output = malloc(size);
   
   memcpy(output, in, total);
   return output;
}

void serializacao_da_tupla_registro(void)
{
   struct TuplaRegistro Z, X = {
      .dados = {
         .criacao          = 1,
         .alteracao        = 2,
         .tamanho_em_bytes = 3,
         .qtd_de_variaveis = 4 
      },

      .agregado = {
         .insercoes    = 5.67,
         .remocoes     = 8.910,
         .atualizacoes = 11.12,
         .consultas    = 13.14
      },

      .tempo = 15161718
   };
   uint8_t* Y = serializa_tr(&X);
   
   debug_tuplaregistro(&X);
   Z = deserializa_tr(Y);
   // visualiza_array_de_bytes(&Y, true);
   debug_tuplaregistro(&Z);
   free(Y);
}

struct TuplaRegistro* cria_array_tr(int n)
{
   struct TuplaRegistro* array;
   const int size = sizeof(struct TuplaRegistro);
   array = calloc(n, size);

   for (int p = 1; p <= n; p++)
   {
      float m = (float)inteiro_positivo(2, 10);

      array[p - 1] = (struct TuplaRegistro) {
         .dados = {
            .criacao          = inteiro_positivo(1, 10),
            .alteracao        = inteiro_positivo(1, 10),
            .tamanho_em_bytes = inteiro_positivo(1, 10),
            .qtd_de_variaveis = inteiro_positivo(1, 10) 
         },
         .agregado = {
            .insercoes    = inteiro_positivo(30, 40) / m,
            .remocoes     = inteiro_positivo(30, 40) / m,
            .atualizacoes = inteiro_positivo(30, 40) / m,
            .consultas    = inteiro_positivo(30, 40) / m
         },
         .tempo = inteiro_positivo(90, 100)
      };
   };
   return array;
}

void serializacao_do_tipo_dinamico_historico(void)
{
   struct Bytes data;
   const int n = 9;
   Historico output, input = { .array=cria_array_tr(n), .total=n };

   puts("A versão antes da serialização:");

   debug_historico(&input);
   data = serializa_historico(&input);

   puts("\nA versão depois da serialização:");
   output = deserializa_historico(&data);
   debug_historico(&output);

   free(output.array);
   free(input.array);
   free_bytes(&data);
}

void serializacao_do_registro(void)
{
   wchar_t* texto = L"Maçã (\U0001f34e)";
   Registro obj = {
      .chave = (struct String){ texto, wcslen(texto) },
      .valor = { .decimal = 5.99 },
      .tipo = Decimal,
      .criacao = 1999,
      .atualizacao = 2002
   };
   debug_registro(&obj);

   struct Bytes seq = serializa_registro(&obj);
   puts("\nBytes do registro acima:");
   print_array(seq.bytes, seq.total, true);
   Registro out = deserializa_registro(&seq);
   debug_registro(&out);

   free_bytes(&seq);
   free(out.chave.array);
}

static HashTable cria_tabela_com_valores_aleatorios(const int Q)
{
   HashTable table = cria_ht(registro_hash, registro_eq);
   const int sz = sizeof(Registro);

   assert(vazia_ht(table));
   for (int i = 1; i <= Q; i++)
   {
      Registro* obj = malloc(sz);

      *obj = random_registro();
      wchar_t* key = obj->chave.array;
      generico_t value = obj;

      insere_ht(table, key, value);
   }
   printf("Quantidade de registros: %zu\n", tamanho_ht(table));
   return table;
}

void serializacao_da_tabela_de_conteudo(void)
{
   HashTable M = cria_tabela_com_valores_aleatorios(10); 
   // IteradorHT I = cria_iter_ht(M);
   struct Bytes content = serializa_tabela(M);

   assert(tamanho_ht(M) == 10);

   /* Primeiro coloca o 'magic number'(o identificador do tipo de arquvio),
   * então os bytes do conteúdo.*/
   puts("\nVisualização do conteúdo serializado:");
   print_array(content.bytes, content.total, true);

   HashTable D = deserializa_tabela(&content);
   assert(tamanho_ht(D) == 10);
}

int main(void)
{
   setlocale(LC_CTYPE, "");

   executa_testes_a (
      true, 6, 
      serializacao_do_tipo_metadados, true,
      processo_de_serializacao_do_saldo, true,
      serializacao_da_tupla_registro, false,
      serializacao_do_tipo_dinamico_historico, true,
      serializacao_do_registro, true,
      serializacao_da_tabela_de_conteudo, true
   );
   return EXIT_SUCCESS;
}
#endif

