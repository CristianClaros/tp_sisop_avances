#ifndef KERNEL_SRC_CONSOLA_H_
#define KERNEL_SRC_CONSOLA_H_

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "../../utils/src/utils/conexion.h"

int iniciar_consola(int socket_memoria);
void analizar_comando(char* linea, int socket_memoria);
void iniciar_proceso(char* rlinea_comando);

#endif /* KERNEL_SRC_CONSOLA_H_ */
