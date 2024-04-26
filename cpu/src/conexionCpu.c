#include "conexionCpu.h"

#define IP_CPU "127.0.0.1"
#define NAME_SERVER "CPU_DISPATCH"

int socket_dispatch;
int socket_interrupt;
int socket_memoria;

int iniciar_cpu(t_config_cpu* cpu_datos, t_log* logger_cpu){

	//Conectar a Memoria
	socket_memoria = crear_conexion(logger_cpu, IP_CPU, cpu_datos->IP_MEMORIA, cpu_datos->PUERTO_MEMORIA);
	enviar_mensaje("CPU",socket_memoria, HANDSHAKE);

	//SERVIDOR CPU_INTERRUPT
	socket_interrupt = iniciar_servidor(logger_cpu, "CPU_INTERRUPT", IP_CPU, cpu_datos->PUERTO_ESCUCHA_INTERRUPT);
	//crearServidor(logger_cpu, "SERVER_CPU_INTERRUPT", IP_CPU, cpu_datos->PUERTO_ESCUCHA_INTERRUPT, socket_interrupt, procesar_conexion);

	//Servidor CPU_DISPATCH
	socket_dispatch = iniciar_servidor(logger_cpu, NAME_SERVER, IP_CPU, cpu_datos->PUERTO_ESCUCHA_DISPATCH);

	while(server_escuchar(logger_cpu, NAME_SERVER, socket_dispatch));

	return 1;
}
