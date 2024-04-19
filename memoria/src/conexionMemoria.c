#include "conexionMemoria.h"

#define IP_MEMORIA "127.0.0.1"

int socket_memoria;

int iniciar_memoria(t_config_memoria* memoria_datos, t_log* logger_memoria){

	crearServidor(logger_memoria, "SERVER MEMORIA", IP_MEMORIA, memoria_datos->PUERTO_ESCUCHA, socket_memoria, procesar_conexion);


	return 1;
}
