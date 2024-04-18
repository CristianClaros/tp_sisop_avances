#include "conexionEntradaSalida.h"

#define IP_INTERFAZ "127.0.0.1"
#define PUERTO_INTERFAZ "8003"

int socket_interfaz;

int iniciar_interfaz(t_config_entradasalida* entradasalida_datos, t_log* logger_entradasalida){
	crear_conexion(logger_entradasalida, "KERNEL", entradasalida_datos->IP_KERNEL, entradasalida_datos->PUERTO_KERNEL);
	crear_conexion(logger_entradasalida, "MEMORIA", entradasalida_datos->IP_MEMORIA, entradasalida_datos->PUERTO_MEMORIA);

	crearServidor(logger_entradasalida, "INTERFAZ", IP_INTERFAZ, PUERTO_INTERFAZ, socket_interfaz, procesar_conexion);

	return 1;
}
