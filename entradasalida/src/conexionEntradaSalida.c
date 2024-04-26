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

	while(server_escuchar(logger_entradasalida, NAME_SERVER, socket_interfaz));

	return 1;
}
