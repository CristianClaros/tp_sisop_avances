#ifndef MEMORIA_SRC_CONEXIONMEMORIA_H_
#define MEMORIA_SRC_CONEXIONMEMORIA_H_

#include "../../utils/src/utils/conexion.h"

typedef struct{
	int cant_caracteres;
	char* parametro;
}t_parametro;

typedef struct{
	char* instruccion;
	int cant_parametros;
	t_list* parametros;
}t_instruccion;

int iniciar_memoria(t_config_memoria* memoria_datos, t_log* logger_memoria);
void* procesar_conexion_memoria(void* args);
void iterator_instruccion(t_instruccion* instruccion);
void iterator_parametro(t_parametro* param);
int cantidad_argumentos(char* token);
void abrir_instrucciones(char* ruta);

#endif /* MEMORIA_SRC_CONEXIONMEMORIA_H_ */
