#include "conexionEntradaSalida.h"

#define IP_INTERFAZ "127.0.0.1"
#define PUERTO_INTERFAZ "8008"
#define NAME_SERVER "SERVER INTERFAZ"

int socket_interfaz;
int socket_memoria;
int socket_kernel;

int iniciar_interfaz(t_config_entradasalida* entradasalida_datos, t_log* logger_entradasalida){

	socket_interfaz = iniciar_servidor(logger_entradasalida, NAME_SERVER, IP_INTERFAZ, PUERTO_INTERFAZ);

	socket_memoria = crear_conexion(logger_entradasalida, NAME_SERVER, entradasalida_datos->IP_MEMORIA, entradasalida_datos->PUERTO_MEMORIA);
	enviar_mensaje(entradasalida_datos->TIPO_INTERFAZ, socket_memoria, HANDSHAKE);

	socket_kernel = crear_conexion(logger_entradasalida, NAME_SERVER, entradasalida_datos->IP_KERNEL, entradasalida_datos->PUERTO_KERNEL);
	enviar_mensaje(entradasalida_datos->TIPO_INTERFAZ, socket_kernel, HANDSHAKE);

	while(server_escuchar(logger_entradasalida, NAME_SERVER, socket_interfaz, procesar_conexion_entradasalida));

	return 1;
}

// Procesar conexion entradasalida
void* procesar_conexion_entradasalida(void* void_args){

    t_procesar_conexion_args *args = (t_procesar_conexion_args *) void_args;
    t_log *logger = args->log;
    int cliente_socket = args->fd;
    char *server_name = args->server_name;
    free(args);

    op_code cop;
    while (cliente_socket != -1) {

        if (recv(cliente_socket, &cop, sizeof(op_code), 0) != sizeof(op_code)) {
            log_info(logger, "DISCONNECT!");
            return NULL;
        }
        switch (cop) {

        	//---------------------------------------- BASICAS -----------------------
    		case HANDSHAKE:
    			//Es el saludo del modulo para saber si se conecto correctamente
    			recibir_mensaje(cliente_socket);
    			break;
        	//----------------------------------------INTERFAZ----------------------
        	case IO_GEN_SLEEP:
        		//Recibe orden del Kernel para SLEEP
        		break;
        	case IO_STDIN_READ:
        		//Recibe orden del Kernel para leer por interfaz
        		break;
        	case IO_STDOUT_WRITE:
        		//Recibe orden del Kernel para mostrar por interfaz
        		break;
        	case IO_FS_CREATE:
        		//Recibe orden del Kernel para crear un FS
        		break;
        	case IO_FS_DELETE:
        		//Recibe orden del Kernel para borra un FS
        		break;
        	case IO_FS_TRUNCATE:
        		//Recibe orden del Kernel para truncar un FS
        		break;
        	case IO_FS_WRITE:
        		///Recibe orden del Kernel para escribir en un FS
        		break;
        	case IO_FS_READ:
        		///Recibe orden del Kernel para leer en un FS
        		break;
        	// --------------------- DESCONEXION -------------------------
            case DESCONEXION:
            	//ERROR DE DESCONEXION
                log_error(logger, "Cliente desconectado de %s...", server_name);
                break;
            default:
                log_error(logger, "Algo anduvo mal en el server de %s", server_name);
                log_info(logger, "Cop: %d", cop);
                break;
        }
    }

    log_warning(logger, "El cliente se desconecto de %s server", server_name);
    return NULL;
}
