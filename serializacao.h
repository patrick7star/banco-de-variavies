
#ifndef _SERIALIZACAO_H
#define _SERIALIZACAO_H
#include <stddef.h>
#include <inttypes.h>
// Módulos do próprio projeto.
#include "banco.h"

 struct Bytes serializa_historico   (Historico*     in);
 Historico    deserializa_historico (struct Bytes*  in);

#endif
