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

int iniciar_memoria(t_config_memoria* memoria_datos, t_log* logger_memoria);
void* procesar_conexion_memoria(void* args);
int cantidad_argumentos(char* token);

t_list* abrir_instrucciones(char* ruta);
void iterator_pid_instruccion(t_pcb_instrucciones* pcb_instrucciones);

#endif /* MEMORIA_SRC_CONEXIONMEMORIA_H_ */
