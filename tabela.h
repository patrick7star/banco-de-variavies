
#ifndef __TABELA_H__
#define __TABELA_H__
#include <stddef.h>
#include <stdbool.h>
#include "hashtable_ref.h"

 /* Dois modos de calcular o hash da chave do dicionário, entretanto se 
  * diferenciam em desempenho. */
 size_t registro_hash (generico_t k, size_t cp);
 size_t registro_hash_i (generico_t k, size_t cp);

 /* Verificam se dois valores de estringues unicodes(wide strings) são 
  * iguais. */
 bool registro_eq (generico_t a, generico_t b);

#endif
