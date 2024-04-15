#ifndef MEMORIA_SRC_MAIN_H_
#define MEMORIA_SRC_MAIN_H_

#include "../../utils/src/utils/logger.h"
#include "../../utils/src/utils/config.h"
#include "conexionMemoria.h"


t_config_memoria* datos_memoria(char* ruta_config, t_log* logger);
t_config_memoria* cargar_config_memoria(t_config* config, t_log* logger);
int eliminar_datos_memoria(t_config_memoria* datos_memoria);

#endif /* MEMORIA_SRC_MAIN_H_ */
