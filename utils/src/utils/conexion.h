#ifndef UTILS_SRC_UTILS_CONEXION_H_
#define UTILS_SRC_UTILS_CONEXION_H_

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

typedef enum
{
	//+++++++++++++++++++ BASICOS
	DESCONEXION = -1,
	HANDSHAKE = 0,
	//+++++++++++++++++++++++	KERNEL
	INICIAR_PROCESO,
	FINALIZAR_PROCESO,
	INICIAR_PLANIFICACION,
	DETENER_PLANIFICACION,
	EJECUTAR_SCRIP_OPERACIONES,
	MODIFICAR_GRADO_MULTIPROGRAMACION,
	LISTAR_PROCESOS_ESTADO,
	IDENTIFICAR_INTERFAZ,
	RECIBIR_PROCESO_DESALOJADO,
	FINALIZAR_PROCESO_SOLICITUD,
	FINALIZAR_PROCESO_ERROR,
	FINALIZAR_PROCESO_CONSOLA,
	SOLICITUD_RECURSO,
	LIBERAR_RECURSO,
	SLEEP_INTERFAZ,
	STDIN_INTERFAZ,
	STDOUT_INTERFAZ,
	CREATE_FS_INTERFAZ,
	DELETE_FS_INTERFAZ,
	TRUNCATE_FS_INTERFAZ,
	READ_FS_INTERFAZ,
	WRITE_FS_INTERFAZ,
	//+++++++++++++++++++++++	CPU
	EJECUTAR_PROCESO,
	RECIBIR_DIRECCION_FISICA,
	RECIBIR_INSTRUCCION,
	//+++++++++++++++++++++++	MEMORIA
	CREAR_PROCESO,
	LIBERAR_PROCESO,
	TRADUCIR_DIRECCION_LOGICA,
	PEDIR_INSTRUCCION,
	AJUSTAR_TAMANIO_PROCESO,
	OBTENER_FRAME,
	LEER_DIRECCION_FISICA,
	//+++++++++++++++++++++++	INTERFAZ
	IO_GEN_SLEEP,
	IO_STDIN_READ,
	IO_STDOUT_WRITE,
	IO_FS_CREATE,
	IO_FS_DELETE,
	IO_FS_TRUNCATE,
	IO_FS_WRITE,
	IO_FS_READ,
} op_code;

//Tipo struct para conexion
typedef struct {
    t_log *log;
    int fd;
    char *server_name;
} t_procesar_conexion_args;

//Coenxion paquete
typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

void* procesar_conexion(void* void_args);

void crearServidor(t_log* logger, char* name_server, char* ip_server, char* puerto_server, int socket_server, void (*procesar_conexion)(t_procesar_conexion_args* args));
int iniciar_servidor(t_log* logger, const char* name_server, char* ip_server, char* puerto_server);
int server_escuchar(t_log *logger, char* server_name, int socket_server);
int esperar_cliente(t_log* logger, const char* name, int socket_servidor);

int crear_conexion(t_log* logger, const char* server_name, char* ip, char* puerto);


void* recibir_buffer(int*, int);
void recibir_mensaje(int);
int recibir_operacion(int);

void enviar_mensaje(char* mensaje, int socket_cliente, int codigo_protocolo);
t_paquete* crear_paquete(int codigo_protocolo);
void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio);
void enviar_paquete(t_paquete* paquete, int socket_cliente);
void eliminar_paquete(t_paquete* paquete);

#endif /* UTILS_SRC_UTILS_CONEXION_H_ */
