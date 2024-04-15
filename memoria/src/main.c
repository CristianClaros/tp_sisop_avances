#include <stdlib.h>
#include <stdio.h>

#include "main.h"


#define ARGUMENTOS 2

int main(int argc, char* argv[]) {

	t_log* logger_memoria;
	t_config_memoria* memoria_datos;

	if(argc != ARGUMENTOS){
			perror("ERROR EN LA CANTIDAD DE ARGUMENTOS");
			return -1;
	}

	if(!(logger_memoria = iniciar_logger("../memoria.log", "LOGGER_MEMORIA")) || !(memoria_datos = datos_memoria(argv[1],logger_memoria))){
		perror("ERROR AL CARGAR DATOS!!!\n");
		return -1;
	}

	iniciar_memoria(memoria_datos, logger_memoria);

	eliminar_logger(logger_memoria);
	eliminar_datos_memoria(memoria_datos);

    return 0;
}


//Carga en una struct Kernel los datos de config

t_config_memoria* datos_memoria(char* ruta_config, t_log* logger){
	t_config_memoria* memoria_struct;
	t_config* config;


	if((config = iniciar_config(ruta_config, logger))){
		memoria_struct = cargar_config_memoria(config, logger);
		eliminar_config(config, logger);
	}else{
		return NULL;
	}

	return memoria_struct;

}

t_config_memoria* cargar_config_memoria(t_config* config, t_log* logger){
	t_config_memoria* memoria_struct = malloc(sizeof(t_config_memoria));

	memoria_struct->PUERTO_ESCUCHA = strdup(config_get_string_value(config, "PUERTO_ESCUCHA"));
	log_info(logger, "PUERTO_ESCUCHA: %s", memoria_struct->PUERTO_ESCUCHA);

	memoria_struct->TAM_MEMORIA = config_get_int_value(config, "TAM_MEMORIA");
	log_info(logger, "TAM_MEMORIA: %i", memoria_struct->TAM_MEMORIA);

	memoria_struct->TAM_PAGINA = config_get_int_value(config, "TAM_PAGINA");
	log_info(logger, "TAM_PAGINA: %i", memoria_struct->TAM_PAGINA);

	memoria_struct->PATH_INSTRUCCIONES = strdup(config_get_string_value(config, "PATH_INSTRUCCIONES"));
	log_info(logger, "PATH_INSTRUCCIONES: %s", memoria_struct->PATH_INSTRUCCIONES);

	memoria_struct->RETARDO_RESPUESTA = config_get_int_value(config, "RETARDO_RESPUESTA");
	log_info(logger, "RETARDO_RESPUESTA: %i", memoria_struct->RETARDO_RESPUESTA);

	return memoria_struct;
}

int eliminar_datos_memoria(t_config_memoria* datos_memoria){
	free(datos_memoria->PUERTO_ESCUCHA);
	free(datos_memoria->PATH_INSTRUCCIONES);

	free(datos_memoria);

	return 1;
}
