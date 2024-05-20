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
#include<semaphore.h>
#include<threads.h>
#include<pthread.h>

#include "../../utils/src/utils/conexion.h"

int iniciar_kernel(t_config_kernel* kernel_datos, t_log* logger_kernel);
int iniciar_consola();
void analizar_comando(char* linea);
void inicializar_variables();
t_list* iniciar_recursos();
void* procesar_conexion_kernel(void* args);
void* iniciar_proceso(char* ruta);
void* cambiar_estado(t_proceso* proceso, char* estado);
bool buscar_pid(t_proceso* proceso);

void iterator(t_proceso* proceso);

#endif /* KERNEL_SRC_CONEXIONKERNEL_H_ */
