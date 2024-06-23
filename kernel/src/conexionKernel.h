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
#include "../../utils/src/utils/estructuras.h"

int iniciar_kernel(t_config_kernel* kernel_datos, t_log* logger_kernel);
int iniciar_consola();
void analizar_comando(char* linea);
void inicializar_variables();
t_list* iniciar_recursos();
void iniciar_planificador_corto();
void iniciar_fifo();
void iniciar_rr();
void iniciar_vrr();

void* procesar_conexion_kernel(void* args);
void* iniciar_proceso(char* ruta);
void* cambiar_estado(t_proceso* proceso, char* estado);

void iterator(t_proceso* proceso);
bool buscar_interfaz(t_interfaz* interfaz);
bool buscar_pid(t_proceso* proceso);

#endif /* KERNEL_SRC_CONEXIONKERNEL_H_ */
