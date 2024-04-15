#ifndef UTILS_SRC_UTILS_LOGGER_H_
#define UTILS_SRC_UTILS_LOGGER_H_

#include<stdio.h>
#include<commons/log.h>
#include<commons/config.h>
#include<string.h>
#include<stdlib.h>

t_log* iniciar_logger(char* ruta_log, char* name_log);
int eliminar_logger(t_log* logger);

#endif /* UTILS_SRC_UTILS_LOGGER_H_ */
