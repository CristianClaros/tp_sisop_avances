#include "logger.h"

t_log* iniciar_logger(char* ruta_log, char* name_log){
	t_log* logger = log_create(ruta_log, name_log, 1, LOG_LEVEL_INFO);

	if(!logger){
		perror("ERROR AL INICIAR EL LOGGER!!\n");
		return NULL;
	}
	log_info(logger, "LOGGER CREADO CORRECTAMENTE!!!\n");

	return logger;
}

int eliminar_logger(t_log* logger){
	if(logger){
		log_destroy(logger);
		perror("LOGGER BORRADO CORRECTAMENTE!!!");
		return 0;
	}
	return 1;
}

