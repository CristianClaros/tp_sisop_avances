#ifndef ENTRADASALIDA_SRC_CONEXIONENTRADASALIDA_H_
#define ENTRADASALIDA_SRC_CONEXIONENTRADASALIDA_H_

#include "../../utils/src/utils/conexion.h"
#include "../../utils/src/utils/estructuras.h"

int iniciar_interfaz(t_config_entradasalida* entradasalida_datos, t_log* logger_entradasalida);
void* procesar_conexion_entradasalida(void* args);

void ejecutar_interfaz_generica(int valor, int pid, t_log* logger);
void ejecutar_interfaz_monitor(int pid, int registro, int tamanio, t_log* logger);
void ejecutar_interfaz_teclado(int pid, int registro, int tamanio, t_log* logger);


#endif /* ENTRADASALIDA_SRC_CONEXIONENTRADASALIDA_H_ */
