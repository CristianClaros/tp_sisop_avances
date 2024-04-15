#include <stdlib.h>
#include <stdio.h>

#include "main.h"


#define ARGUMENTOS 2

int main(int argc, char* argv[]) {

	t_log* logger_entradasalida;
	t_config_entradasalida* entradasalida_datos;

	if(argc != ARGUMENTOS){
			perror("ERROR EN LA CANTIDAD DE ARGUMENTOS");
			return -1;
	}

	if(!(logger_entradasalida = iniciar_logger("../entradasalida.log", "LOGGER_ENTRADASALIDA")) || !(entradasalida_datos = datos_entradasalida(argv[1],logger_entradasalida))){
		perror("ERROR AL CARGAR DATOS!!!\n");
		return -1;
	}

	eliminar_logger(logger_entradasalida);
	eliminar_datos_entradasalida(entradasalida_datos);

    return 0;
}


//Carga en una struct Kernel los datos de config

t_config_entradasalida* datos_entradasalida(char* ruta_config, t_log* logger){
	t_config_entradasalida* entradasalida_struct;
	t_config* config;


	if((config = iniciar_config(ruta_config, logger))){
		entradasalida_struct = cargar_config_entradasalida(config, logger);
		eliminar_config(config, logger);
	}else{
		return NULL;
	}

	return entradasalida_struct;

}

t_config_entradasalida* cargar_config_entradasalida(t_config* config, t_log* logger){
	t_config_entradasalida* entradasalida_struct = malloc(sizeof(t_config_entradasalida));


	entradasalida_struct->TIPO_INTERFAZ = strdup(config_get_string_value(config, "TIPO_INTERFAZ"));
	log_info(logger, "TIPO_INTERFAZ: %s", entradasalida_struct->TIPO_INTERFAZ);

	entradasalida_struct->TIEMPO_UNIDAD_TRABAJO = config_get_int_value(config, "TIEMPO_UNIDAD_TRABAJO");
	log_info(logger, "TIEMPO_UNIDAD_TRABAJO: %i", entradasalida_struct->TIEMPO_UNIDAD_TRABAJO);

	entradasalida_struct->IP_KERNEL = strdup(config_get_string_value(config, "IP_KERNEL"));
	log_info(logger, "IP_KERNEL: %s", entradasalida_struct->IP_KERNEL);

	entradasalida_struct->PUERTO_KERNEL = strdup(config_get_string_value(config, "PUERTO_KERNEL"));
	log_info(logger, "PUERTO_KERNEL: %s", entradasalida_struct->PUERTO_KERNEL);

	entradasalida_struct->IP_MEMORIA = strdup(config_get_string_value(config, "IP_MEMORIA"));
	log_info(logger, "IP_MEMORIA: %s", entradasalida_struct->IP_MEMORIA);

	entradasalida_struct->PUERTO_MEMORIA = strdup(config_get_string_value(config, "PUERTO_MEMORIA"));
	log_info(logger, "PUERTO_MEMORIA: %s", entradasalida_struct->PUERTO_MEMORIA);

	entradasalida_struct->PATH_BASE_DIALFS = strdup(config_get_string_value(config, "PATH_BASE_DIALFS"));
	log_info(logger, "PATH_BASE_DIALFS: %s", entradasalida_struct->PATH_BASE_DIALFS);

	entradasalida_struct->BLOCK_SIZE = config_get_int_value(config, "BLOCK_SIZE");
	log_info(logger, "BLOCK_SIZE: %i", entradasalida_struct->BLOCK_SIZE);

	entradasalida_struct->BLOCK_COUNT = config_get_int_value(config, "BLOCK_COUNT");
	log_info(logger, "BLOCK_COUNT: %i", entradasalida_struct->BLOCK_COUNT);

	return entradasalida_struct;
}

int eliminar_datos_entradasalida(t_config_entradasalida* datos_entradasalida){

	free(datos_entradasalida->IP_KERNEL);
	free(datos_entradasalida->PUERTO_KERNEL);
	free(datos_entradasalida->IP_MEMORIA);
	free(datos_entradasalida->PUERTO_MEMORIA);
	free(datos_entradasalida->PATH_BASE_DIALFS);

	free(datos_entradasalida);

	return 1;
}
