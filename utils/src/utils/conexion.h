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

#include "config.h"

typedef enum
{
    //----------------BASICOS--------------------------------
    HANDSHAKE = 1,
    //-------------MENSAJES ENTRE CONSOLA-KERNEL------------------------------------
    GESTIONAR_CONSOLA_NUEVA = 0,
    SOLICITAR_VALOR,
    IMPRIMIR_VALOR,
    PROCESO_TERMINADO,
    //------------------------------------------------------------------------------
    //-------------MENSAJES SERIALIZACION GLOBALES---------------------------------
    PAQUETE = 4,
    MENSAJE = 5,
    PCB = 30,
    //MENSAJES KERNEL - MEMORIA
    TABLA_SEGMENTOS_INICIAL,
    //-------------MENSAJES MEMORIA - KERNEL---------------------------------
    PROCESO_INICIADO,
    CREAR_SEGMENTO,
    SEGMENTO_CREADO,
    ESPACIO_INSUFICIENTE,
    COMPACTACION,
    BORRAR_SEGMENTO,
	DESCONEXION
} op_code;

//Tipo struct para conexion
typedef struct {
    t_log *log;
    int fd;
    char *server_name;
} t_procesar_conexion_args;

void crearServidor(t_log* logger, char* name_server, char* ip_server, char* puerto_server, int socket_server, void (*procesar_conexion)(t_procesar_conexion_args* args));
int iniciar_servidor(t_log* logger, const char* name_server, char* ip_server, char* puerto_server);
int server_escuchar(t_log *logger, char* server_name, int socket_server, void (*procesar_conexion)(t_procesar_conexion_args* args));
int esperar_cliente(t_log* logger, const char* name, int socket_servidor);

int crear_conexion(t_log* logger, const char* server_name, char* ip, char* puerto);

#endif /* UTILS_SRC_UTILS_CONEXION_H_ */
