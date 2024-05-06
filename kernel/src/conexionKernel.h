#ifndef KERNEL_SRC_CONEXIONKERNEL_H_
#define KERNEL_SRC_CONEXIONKERNEL_H_

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/string.h>

#include "../../utils/src/utils/conexion.h"

int iniciar_kernel(t_config_kernel* kernel_datos, t_log* logger_kernel);
int iniciar_consola();
void analizar_comando(char* linea);
void inicializar_variables();
t_list* iniciar_recursos();

#endif /* KERNEL_SRC_CONEXIONKERNEL_H_ */
