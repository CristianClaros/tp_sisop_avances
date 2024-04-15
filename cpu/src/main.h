#ifndef CPU_SRC_MAIN_H_
#define CPU_SRC_MAIN_H_

#include "../../utils/src/utils/logger.h"
#include "../../utils/src/utils/config.h"
#include "conexionCpu.h"


t_config_cpu* datos_cpu(char* ruta_config, t_log* logger);
t_config_cpu* cargar_config_cpu(t_config* config, t_log* logger);
int eliminar_datos_cpu(t_config_cpu* datos_cpu);

#endif /* CPU_SRC_MAIN_H_ */
