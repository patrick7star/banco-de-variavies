/*   Serialização/ e deserilização de vários tipos primitivos, ou estruturas
 * um pouco mais complexas, necessárias para este programa. 
 *   A ordem de arranjo dos bytes de cada campo de variáveis segue, 
 * estritamente, a ordem com que está declarada na implementação deste tipo
 * de estrutura abstrata. 
 */

#ifndef _SERIALIZACAO_H
#define _SERIALIZACAO_H
#include <stddef.h>
#include <inttypes.h>
// Módulos do próprio projeto.
#include "banco.h"

 struct Bytes serializa_historico   (Historico*     in);
 Historico    deserializa_historico (struct Bytes*  in);

#endif
