#include "conexionMemoria.h"

#define IP_MEMORIA "127.0.0.1"
#define NAME_SERVER "SERVER MEMORIA"

int socket_memoria;

int iniciar_memoria(t_config_memoria* memoria_datos, t_log* logger_memoria){

	socket_memoria = iniciar_servidor(logger_memoria, NAME_SERVER, IP_MEMORIA, memoria_datos->PUERTO_ESCUCHA);

	while(server_escuchar(logger_memoria, NAME_SERVER, socket_memoria));

	return 1;
}
