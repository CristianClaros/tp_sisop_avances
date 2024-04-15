#ifndef MEMORIA_SRC_CONEXIONMEMORIA_H_
#define MEMORIA_SRC_CONEXIONMEMORIA_H_

#include "../../utils/src/utils/conexion.h"

#define IP_MEMORIA "127.0.0.1"


int iniciar_memoria(t_config_memoria* memoria_datos, t_log* logger_memoria);
void procesar_conexion_memoria(t_procesar_conexion_args* void_args);

#endif /* MEMORIA_SRC_CONEXIONMEMORIA_H_ */
