#ifndef ENTRADASALIDA_SRC_MAIN_H_
#define ENTRADASALIDA_SRC_MAIN_H_

#include "../../utils/src/utils/logger.h"
#include "../../utils/src/utils/config.h"
#include "conexionEntradaSalida.h"


t_config_entradasalida* datos_entradasalida(char* nombre_interfaz, char* ruta_config, t_log* logger);
t_config_entradasalida* cargar_config_entradasalida(t_config* config, char* nombre_interfaz, t_log* logger);
int eliminar_datos_entradasalida(t_config_entradasalida* datos_entradasalida);

#endif /* ENTRADASALIDA_SRC_MAIN_H_ */
