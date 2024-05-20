#include "conexionMemoria.h"

#define IP_MEMORIA "127.0.0.1"
#define NAME_SERVER "SERVER MEMORIA"

int socket_memoria;

int iniciar_memoria(t_config_memoria* memoria_datos, t_log* logger_memoria){

	socket_memoria = iniciar_servidor(logger_memoria, NAME_SERVER, IP_MEMORIA, memoria_datos->PUERTO_ESCUCHA);

	while(server_escuchar(logger_memoria, NAME_SERVER, socket_memoria, procesar_conexion_memoria));

	return 1;
}

void* procesar_conexion_memoria(void* void_args){

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
        	//----------------------------------------MEMORIA----------------------
        	case CREAR_PROCESO:
        		//Crear la tabla de recursos necesarios para que el proceso este en memoria
        		recibir_mensaje(cliente_socket);
        		enviar_mensaje("TABLA RECIBIDA", cliente_socket, HANDSHAKE);
        		break;
        	case LIBERAR_PROCESO:
        		//Libera los recursos creados para el proceso en la memoria
        		break;
        	case TRADUCIR_DIRECCION_LOGICA:
        		//Traduce las direccion logica a fisica que solicita el CPU
        		break;
        	case PEDIR_INSTRUCCION:
        		//Recibe la orden de enviar la instruccion a ejcutar que solicto el CPU
        		break;
        	case AJUSTAR_TAMANIO_PROCESO:
        		//Modifica el tamaño de un proceso solicitado por CPU
        		break;
        	case OBTENER_FRAME:
        		//Espera solicut de FRAME por el CPU
        		break;
        	case LEER_DIRECCION_FISICA:
        		//Lee una direccion fisica pedida por el CPU
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
