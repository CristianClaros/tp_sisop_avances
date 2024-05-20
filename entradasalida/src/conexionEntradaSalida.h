#ifndef ENTRADASALIDA_SRC_CONEXIONENTRADASALIDA_H_
#define ENTRADASALIDA_SRC_CONEXIONENTRADASALIDA_H_

#include "../../utils/src/utils/conexion.h"

int iniciar_interfaz(t_config_entradasalida* entradasalida_datos, t_log* logger_entradasalida);
void* procesar_conexion_entradasalida(void* args);

#endif /* ENTRADASALIDA_SRC_CONEXIONENTRADASALIDA_H_ */
