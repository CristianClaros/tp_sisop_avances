#include "conexionCpu.h"

#define IP_CPU "127.0.0.1"

int socket_dispatch;
int socket_interrupt;

int iniciar_cpu(t_config_cpu* cpu_datos, t_log* logger_cpu){

	//Conectar a Memoria
	crear_conexion(logger_cpu, IP_CPU, cpu_datos->IP_MEMORIA, cpu_datos->PUERTO_MEMORIA);

	//Servidor CPU_DISPATCH
	crearServidor(logger_cpu, "SERVER_CPU_DISPATCH", IP_CPU, cpu_datos->PUERTO_ESCUCHA_DISPATCH, socket_dispatch, procesar_conexion);
	//SERVIDOR CPU_INTERRUPT
	crearServidor(logger_cpu, "SERVER_CPU_INTERRUPT", IP_CPU, cpu_datos->PUERTO_ESCUCHA_INTERRUPT, socket_interrupt, procesar_conexion);

	return 1;
}
