#include "conexionKernel.h"

#define IP_KERNEL "127.0.0.1"
#define PUERTO_KERNEL "8003"

int socket_kernel;

int iniciar_kernel(t_config_kernel* kernel_datos, t_log* logger_kernel){
	crear_conexion(logger_kernel, "CPU_DISPATCH", kernel_datos->IP_CPU, kernel_datos->PUERTO_CPU_DISPATCH);
	crear_conexion(logger_kernel, "CPU_INTERRUPT", kernel_datos->IP_CPU, kernel_datos->PUERTO_CPU_INTERRUPT);
	crear_conexion(logger_kernel, "MEMORIA", kernel_datos->IP_MEMORIA, kernel_datos->PUERTO_MEMORIA);

	crearServidor(logger_kernel, "SERVER_KERNEL", IP_KERNEL, PUERTO_KERNEL, socket_kernel, procesar_conexion);


	return 1;
}
