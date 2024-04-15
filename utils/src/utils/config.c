#include "config.h"

t_config* iniciar_config(char* ruta_config, t_log* logger){
	t_config* config = config_create(ruta_config);

	if(!config){
		perror("ERROR AL CREAR EL CONFIG!!!\n");
		return NULL;
	}
	return config;
}

int eliminar_config(t_config* config, t_log* logger){
	if(!config){
		perror("CONFIG NUNCA FUE CREADO!!!\n");
		return -1;
	}

	config_destroy(config);
	log_info(logger, "CONFIG BORRADO CORRECTAMENTE!!!\n");
	return 1;
}

