#ifndef MEMORIA_SRC_CONEXIONMEMORIA_H_
#define MEMORIA_SRC_CONEXIONMEMORIA_H_

#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>

#include "../../utils/src/utils/conexion.h"
#include "../../utils/src/utils/estructuras.h"

typedef struct{
	uint32_t pid;
	t_list* instrucciones;
}t_pcb_instrucciones;

typedef struct{
	uint32_t pid;
	char* ruta_instrucciones;
}t_pid_instruccion;

int iniciar_memoria(t_config_memoria* memoria_datos, t_log* logger_memoria);
void* procesar_conexion_memoria(void* args);

void iterator_pid_instruccion(t_pcb_instrucciones* pcb_instrucciones);

void atender_peticion_instruccion(void* buffer, int cliente_socket);
char* obtenerInstruccion(char* pathInstrucciones, int program_counter);

#endif /* MEMORIA_SRC_CONEXIONMEMORIA_H_ */
