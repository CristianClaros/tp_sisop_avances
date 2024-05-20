#ifndef MEMORIA_SRC_CONEXIONMEMORIA_H_
#define MEMORIA_SRC_CONEXIONMEMORIA_H_

#include "../../utils/src/utils/conexion.h"

int iniciar_memoria(t_config_memoria* memoria_datos, t_log* logger_memoria);
void* procesar_conexion_memoria(void* args);

#endif /* MEMORIA_SRC_CONEXIONMEMORIA_H_ */
