#ifndef UTILS_SRC_UTILS_CONFIG_H_
#define UTILS_SRC_UTILS_CONFIG_H_

#include<commons/config.h>
#include<commons/log.h>
#include<string.h>
#include<stdlib.h>
#include<stdio.h>


typedef struct{
	char* IP_MEMORIA;
	char* PUERTO_MEMORIA;
	char* IP_CPU;
	char* PUERTO_CPU_DISPATCH;
	char* PUERTO_CPU_INTERRUPT;
	char* ALGORITMO_PLANIFICACION;
	int QUANTUM;
	char** RECURSOS;
	char** INSTANCIAS_RECURSOS;
	int GRADO_MULTIPROGRAMACION;
}t_config_kernel;

typedef struct{
	char* IP_MEMORIA;
	char* PUERTO_MEMORIA;
	char* PUERTO_ESCUCHA_DISPATCH;
	char* PUERTO_ESCUCHA_INTERRUPT;
	int CANTIDAD_ENTRADAS_TLB;
	char* ALGORITMO_TLB;
}t_config_cpu;

typedef struct{
	char* PUERTO_ESCUCHA;
	int TAM_MEMORIA;
	int TAM_PAGINA;
	char* PATH_INSTRUCCIONES;
	int RETARDO_RESPUESTA;
}t_config_memoria;

typedef struct{
	char* TIPO_INTERFAZ;
	int TIEMPO_UNIDAD_TRABAJO;
	char* IP_KERNEL;
	char* PUERTO_KERNEL;
	char* IP_MEMORIA;
	char* PUERTO_MEMORIA;
	char* PATH_BASE_DIALFS;
	int BLOCK_SIZE;
	int BLOCK_COUNT;
}t_config_entradasalida;

t_config* iniciar_config(char* ruta_config, t_log* logger);
int eliminar_config(t_config* config, t_log* logger);

#endif /* UTILS_SRC_UTILS_CONFIG_H_ */
