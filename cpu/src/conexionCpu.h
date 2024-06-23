#ifndef CPU_SRC_CONEXIONCPU_H_
#define CPU_SRC_CONEXIONCPU_H_

#include "../../utils/src/utils/conexion.h"
#include "../../utils/src/utils/estructuras.h"

int iniciar_cpu(t_config_cpu* cpu_datos, t_log* logger_cpu);
void* procesar_conexion_cpu(void* args);

char* fetch(t_pcb* pcb, int cliente_socket);
t_instruccion* decode(char* instruccion);
int execute(t_instruccion* instruccion, t_pcb* pcb, int socket_kernel);

int set(t_list* lista_parametros, t_pcb* pcb);
int sum(t_list* lista_parametros, t_pcb* pcb);
int registro_origen(t_pcb* pcb, char* registro_origen);
int sub(t_list* lista_parametros, t_pcb* pcb);
int jnz(t_list* lista_parametros, t_pcb* pcb);

int io_gen_sleep(char* interfaz, int valor, uint32_t pid,int socket_kernel);



#endif /* CPU_SRC_CONEXIONCPU_H_ */
