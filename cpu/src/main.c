#include <stdlib.h>
#include <stdio.h>

#include "main.h"


#define ARGUMENTOS 2

int main(int argc, char* argv[]) {

	t_log* logger_cpu;
	t_config_cpu* cpu_datos;

	if(argc != ARGUMENTOS){
			perror("ERROR EN LA CANTIDAD DE ARGUMENTOS");
			return -1;
	}

	if(!(logger_cpu = iniciar_logger("../cpu.log", "LOGGER_CPU")) || !(cpu_datos = datos_cpu(argv[1],logger_cpu))){
		perror("ERROR AL CARGAR DATOS!!!\n");
		return -1;
	}

	iniciar_cpu(cpu_datos, logger_cpu);

	eliminar_logger(logger_cpu);
	eliminar_datos_cpu(cpu_datos);

    return 0;
}


//Carga en una struct Kernel los datos de config

t_config_cpu* datos_cpu(char* ruta_config, t_log* logger){
	t_config_cpu* cpu_struct;
	t_config* config;


	if((config = iniciar_config(ruta_config, logger))){
		cpu_struct = cargar_config_cpu(config, logger);
		eliminar_config(config, logger);
	}else{
		return NULL;
	}

	return cpu_struct;

}

t_config_cpu* cargar_config_cpu(t_config* config, t_log* logger){
	t_config_cpu* cpu_struct = malloc(sizeof(t_config_cpu));

	cpu_struct->IP_MEMORIA = strdup(config_get_string_value(config, "IP_MEMORIA"));
	log_info(logger, "IP_MEMORIA: %s", cpu_struct->IP_MEMORIA);

	cpu_struct->PUERTO_MEMORIA = strdup(config_get_string_value(config, "PUERTO_MEMORIA"));
	log_info(logger, "PUERTO_MEMORIA: %s", cpu_struct->PUERTO_MEMORIA);

	cpu_struct->PUERTO_ESCUCHA_DISPATCH = strdup(config_get_string_value(config, "PUERTO_ESCUCHA_DISPATCH"));
	log_info(logger, "PUERTO_ESCUCHA_DISPATCH: %s", cpu_struct->PUERTO_ESCUCHA_DISPATCH);

	cpu_struct->PUERTO_ESCUCHA_INTERRUPT = strdup(config_get_string_value(config, "PUERTO_ESCUCHA_INTERRUPT"));
	log_info(logger, "PUERTO_ESCUCHA_INTERRUPT: %s", cpu_struct->PUERTO_ESCUCHA_INTERRUPT);

	cpu_struct->CANTIDAD_ENTRADAS_TLB = config_get_int_value(config, "CANTIDAD_ENTRADAS_TLB");
	log_info(logger, "CANTIDAD_ENTRADAS_TLB: %i", cpu_struct->CANTIDAD_ENTRADAS_TLB);

	cpu_struct->ALGORITMO_TLB = strdup(config_get_string_value(config, "ALGORITMO_TLB"));
	log_info(logger, "ALGORITMO_TLB: %s", cpu_struct->ALGORITMO_TLB);

	return cpu_struct;
}

int eliminar_datos_cpu(t_config_cpu* datos_cpu){
	free(datos_cpu->IP_MEMORIA);
	free(datos_cpu->PUERTO_MEMORIA);
	free(datos_cpu->PUERTO_ESCUCHA_DISPATCH);
	free(datos_cpu->PUERTO_ESCUCHA_INTERRUPT);
	free(datos_cpu->ALGORITMO_TLB);

	free(datos_cpu);

	return 1;
}
