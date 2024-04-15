#include <stdlib.h>
#include <stdio.h>

#include "main.h"


#define ARGUMENTOS 2

int main(int argc, char* argv[]) {

	t_log* logger_kernel;
	t_config_kernel* kernel_datos;

	if(argc != ARGUMENTOS){
			perror("ERROR EN LA CANTIDAD DE ARGUMENTOS");
			return -1;
	}

	if(!(logger_kernel = iniciar_logger("../kernel.log", "LOGGER_KERNEL")) || !(kernel_datos = datos_kernel(argv[1],logger_kernel))){
		perror("ERROR AL CARGAR DATOS!!!\n");
		return -1;
	}

	eliminar_logger(logger_kernel);
	eliminar_datos_kernel(kernel_datos);

    return 0;
}


//Carga en una struct Kernel los datos de config

t_config_kernel* datos_kernel(char* ruta_config, t_log* logger){
	t_config_kernel* kernel_struct;
	t_config* config;


	if((config = iniciar_config(ruta_config, logger))){
		kernel_struct = cargar_config_kernel(config, logger);
		eliminar_config(config, logger);
	}else{
		return NULL;
	}

	return kernel_struct;

}

t_config_kernel* cargar_config_kernel(t_config* config, t_log* logger){
	t_config_kernel* kernel_struct = malloc(sizeof(t_config_kernel));

	char* recursos;
	char* instancias;

	kernel_struct->IP_MEMORIA = strdup(config_get_string_value(config, "IP_MEMORIA"));
	log_info(logger, "IP_MEMORIA: %s", kernel_struct->IP_MEMORIA);

	kernel_struct->PUERTO_MEMORIA = strdup(config_get_string_value(config, "PUERTO_MEMORIA"));
	log_info(logger, "PUERTO_MEMORIA: %s", kernel_struct->PUERTO_MEMORIA);

	kernel_struct->IP_CPU = strdup(config_get_string_value(config, "IP_CPU"));
	log_info(logger, "IP_CPU: %s", kernel_struct->IP_CPU);

	kernel_struct->PUERTO_CPU_DISPATCH = strdup(config_get_string_value(config, "PUERTO_CPU_DISPATCH"));
	log_info(logger, "PUERTO_CPU_DISPATCH: %s", kernel_struct->PUERTO_CPU_DISPATCH);

	kernel_struct->PUERTO_CPU_INTERRUPT = strdup(config_get_string_value(config, "PUERTO_CPU_INTERRUPT"));
	log_info(logger, "PUERTO_CPU_INTERRUPT: %s", kernel_struct->PUERTO_CPU_INTERRUPT);

	kernel_struct->IP_CPU = strdup(config_get_string_value(config, "IP_CPU"));
	log_info(logger, "IP_CPU: %s", kernel_struct->IP_CPU);

	kernel_struct->ALGORITMO_PLANIFICACION = strdup(config_get_string_value(config, "ALGORITMO_PLANIFICACION"));
	log_info(logger, "ALGORITMO_PLANIFICACION: %s", kernel_struct->ALGORITMO_PLANIFICACION);

	kernel_struct->QUANTUM = config_get_int_value(config, "QUANTUM");
	log_info(logger, "QUANTUM: %i", kernel_struct->QUANTUM);

	kernel_struct->RECURSOS = config_get_array_value(config, "RECURSOS");

	//Solo sirve para loguear los recursos
	recursos = config_get_string_value(config, "RECURSOS");
	log_info(logger, "RECURSOS: %s", recursos);

	kernel_struct->INSTANCIAS_RECURSOS = config_get_array_value(config, "INSTANCIAS_RECURSOS");

	//Solo sirve para loguear las instancias
	instancias = config_get_string_value(config, "INSTANCIAS_RECURSOS");
	log_info(logger, "INSTANCIAS_RECURSOS: %s", instancias);

	kernel_struct->GRADO_MULTIPROGRAMACION = config_get_int_value(config, "GRADO_MULTIPROGRAMACION");
	log_info(logger, "GRADO_MULTIPROGRAMACION: %i\n", kernel_struct->GRADO_MULTIPROGRAMACION);

	return kernel_struct;
}

int eliminar_datos_kernel(t_config_kernel* datos_kernel){
	free(datos_kernel->ALGORITMO_PLANIFICACION);
	free(datos_kernel->INSTANCIAS_RECURSOS);
	free(datos_kernel->IP_CPU);
	free(datos_kernel->PUERTO_CPU_DISPATCH);
	free(datos_kernel->IP_MEMORIA);
	free(datos_kernel->PUERTO_CPU_INTERRUPT);
	free(datos_kernel->PUERTO_MEMORIA);
	free(datos_kernel->RECURSOS);

	free(datos_kernel);

	return 1;
}
