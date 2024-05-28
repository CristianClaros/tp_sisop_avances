#ifndef CPU_SRC_CONEXIONCPU_H_
#define CPU_SRC_CONEXIONCPU_H_

#include "../../utils/src/utils/conexion.h"
#include "../../utils/src/utils/estructuras.h"

int iniciar_cpu(t_config_cpu* cpu_datos, t_log* logger_cpu);
void* procesar_conexion_cpu(void* args);

void pedir_instruccion(uint32_t, int socket_cliente, uint32_t codigo_protocolo);

#endif /* CPU_SRC_CONEXIONCPU_H_ */
