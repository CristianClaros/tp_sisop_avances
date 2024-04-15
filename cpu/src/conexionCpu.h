#ifndef CPU_SRC_CONEXIONCPU_H_
#define CPU_SRC_CONEXIONCPU_H_

#include "../../utils/src/utils/conexion.h"

int iniciar_cpu(t_config_cpu* cpu_datos, t_log* logger_cpu);
void procesar_conexion_cpu(t_procesar_conexion_args* void_args);

#endif /* CPU_SRC_CONEXIONCPU_H_ */
