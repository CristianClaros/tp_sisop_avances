#ifndef KERNEL_SRC_MAIN_H_
#define KERNEL_SRC_MAIN_H_

#include "../../utils/src/utils/logger.h"
#include "../../utils/src/utils/config.h"
#include "conexionKernel.h"


t_config_kernel* datos_kernel(char* ruta_config, t_log* logger);
t_config_kernel* cargar_config_kernel(t_config* config, t_log* logger);
int eliminar_datos_kernel(t_config_kernel* datos_kernel);

#endif /* KERNEL_SRC_MAIN_H_ */
