
/* backpropagation simulator header file */
/* From: red_1salida.aspirin */

#include "aspirin_bp.h"


/* NETWORK FUNCTIONS */
extern void red_1salida_set_learning_rate(float x);
extern void red_1salida_set_inertia(float x);
extern float red_1salida_get_learning_rate();
extern float red_1salida_get_inertia();
extern int red_1salida_init_network();
extern void red_1salida_set_random_init_seed(long x);
extern void red_1salida_set_random_init_range(float x);
extern int red_1salida_load_network(char *filename);
extern int red_1salida_dump_network();
extern int red_1salida_ascii_dump_network(char *filename);
extern char *red_1salida_error_string();
extern LB_PTR red_1salida_query_network(int bbindex, int lindex);

/* BLACK BOX FUNCTIONS */

extern void red_1salida_propagate_forward();
extern float red_1salida_calc_error();
extern void red_1salida_calc_grad();
extern void red_1salida_update_weights(float scalar);
extern void red_1salida_set_backprop_counter(int x);
extern int red_1salida_get_backprop_counter();
extern float *red_1salida_get_output();
extern void red_1salida_set_target_output(float *target);
extern float *red_1salida_get_target_output();
extern void red_1salida_set_input(float *input);
extern float *red_1salida_get_input();
/* GENERIC INTERFACE FUNCTIONS (begin with network_) */
extern void network_initialize(char *filename, int verbose);
extern void network_forward(int iterations, void (*generator)(void) );
extern void network_learn(int iterations, void (*generator)(void) );
extern void network_clear_delays();
extern void network_dump(char *filename);
extern void network_load(char *filename);
extern void network_ascii_dump(int formatted);
extern void network_load_ascii();
extern LB_PTR network_query(int bbindex, int lindex);
extern void network_forward_print(int iterations, void (*generator)(void) );
extern void network_forward_pdpfa(int iterations, void (*generator)(void), float threshold);

