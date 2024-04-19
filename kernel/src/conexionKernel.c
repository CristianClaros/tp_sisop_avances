#include "conexionKernel.h"

#define IP_KERNEL "127.0.0.1"
#define PUERTO_KERNEL "8003"

int socket_kernel;
int socket_dispatch;
int socket_interrupt;
int socket_memoria;

int iniciar_kernel(t_config_kernel* kernel_datos, t_log* logger_kernel){
	socket_interrupt = crear_conexion(logger_kernel, IP_KERNEL, kernel_datos->IP_CPU, kernel_datos->PUERTO_CPU_INTERRUPT);
	enviar_mensaje("KERNEL-INTERRUPT", socket_interrupt, HANDSHAKE);

	socket_dispatch = crear_conexion(logger_kernel, IP_KERNEL, kernel_datos->IP_CPU, kernel_datos->PUERTO_CPU_DISPATCH);
	enviar_mensaje("KERNEL-DISPATCH", socket_dispatch, HANDSHAKE);

	socket_memoria = crear_conexion(logger_kernel, IP_KERNEL, kernel_datos->IP_MEMORIA, kernel_datos->PUERTO_MEMORIA);
	enviar_mensaje("KERNEL-DISPATCH", socket_dispatch, HANDSHAKE);

	crearServidor(logger_kernel, "SERVER_KERNEL", IP_KERNEL, PUERTO_KERNEL, socket_kernel, procesar_conexion);


	return 1;
}
