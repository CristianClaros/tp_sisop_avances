#ifndef UTILS_SRC_UTILS_ESTRUCTURAS_H_
#define UTILS_SRC_UTILS_ESTRUCTURAS_H_

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <commons/log.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <inttypes.h>

#include "config.h"

//Proceso
typedef struct{
	//program_counter
	uint32_t program_counter;
	//Registro 1 byte
	uint8_t AX;
	uint8_t BX;
	uint8_t CX;
	uint8_t DX;
	//Registro 4 bytes
	uint32_t EAX;
	uint32_t EBX;
	uint32_t ECX;
	uint32_t EDX;
	//DireccionesLogicas
	uint32_t SI;
	uint32_t DI;
}t_registros;

typedef struct{
	uint32_t pid;
	uint32_t quantum;
	t_registros* registro;
	//t_segmentos segmentos;
	//t_archivos archivos;
}t_pcb;

//estructs para kernel
typedef struct{
	uint32_t socket;
	char* nombre;
	char* tipo_interfaz;
}t_interfaz;

typedef struct{
	uint32_t pid_list;
	char* estado;
	t_pcb* pcb;

}t_proceso;

typedef struct{
	char* parametro;
}t_parametro;

typedef struct{
	char* instruccion;
	uint32_t cant_parametros;
	t_list* parametros;
}t_instruccion;

void iterator_instruccion(t_instruccion* instruccion);
void iterator_parametro(t_parametro* param);

void enviar_pcb(int socket_cliente, t_pcb* pcb, uint32_t protocolo);
t_pcb* recibir_pcb(int socket_cliente);

void mostrar_pcb(t_pcb* pcb);

//Solo para verificar si se recibieron las instrucciones
t_list* recibir_instrucciones(int socket_cliente);
void enviar_instrucciones(int socket_cliente, t_list* lista_instrucciones, uint32_t protocolo);


t_registros*  iniciar_registros();
void mostrar_registros(t_registros* registro);

t_instruccion* recibir_instruccion(int socket_cliente);
void enviar_instruccion(int socket_cliente, t_instruccion* instruccion, uint32_t protocolo);

int cantidad_argumentos(char* token);
t_list* abrir_instrucciones(char* ruta);

#endif /* UTILS_SRC_UTILS_ESTRUCTURAS_H_ */
