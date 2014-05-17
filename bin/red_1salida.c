

/* You may want to run this thru 'cb' or 'indent' */

/**** Back-propagation simulation for red_1salida.aspirin ****/


#include "aspirin_bp.h"


#include "BpSim.h"


/* GLOBAL VARIABLES */
float  BPlearning_rate = 0.2; 
float  BPinertia = 0.95;      
static float  init_range = 0.1; /* feedforward weights */
static float  ar_init_range = 0.001; /* autogressive feedback weights */ 
static long   init_seed = 123;



/* Don't backprop if Error < BPthreshold */
float         BPthreshold = 0.000050;

char error_string[E_STRING_SIZE]; /* describes current error */

/* Connection Buffer for interconnect data */
static CB connection_buffer[2];

/* Communication Buffer */
static LB comms_buffer;

/* NETWORK DATA */
#ifndef PADDING
# define PADDING 0
#endif
static float network_data[3717+PADDING];


/* data for red_1salida */
static unsigned int b0fcounter = 0; /* # times fwd */
static unsigned int b0bcounter = 0; /* # times bkwd */

/* red_1salida:oculto */
#define b0_l1_v (network_data + 0) /* values */
#define b0_l1_t (network_data + 66) /* thresholds */
#define b0_l1_ac (network_data + 98) /* accumulated changes */
#define b0_l1_dt (network_data + 130) /* delta thresholds */
#define b0_l1_c (network_data + 32) /* credit */

/* red_1salida:salida */
#define b0_l0_v (network_data + 64) /* values */
#define b0_l0_t (network_data + 162) /* thresholds */
#define b0_l0_ac (network_data + 163) /* accumulated changes */
#define b0_l0_dt (network_data + 164) /* delta thresholds */
#define b0_l0_c (network_data + 65) /* credit */

static float *b0_input_vector = (float *)NULL; /* input vector */

#define b0_input_vector0 b0_input_vector /* input vector */

static float *b0_target_output = (float *)NULL; /* desired output */

/* red_1salida:oculto input connections */
#define b0_oculto1 (network_data + 165) /* weights from $INPUTS */
#define b0_oculto1ac (network_data + 1317)      /* accumulated changes */
#define b0_oculto1d (network_data + 2469)       /* delta weights */

/* red_1salida:salida input connections */
#define b0_salida2 (network_data + 3621) /* weights from red_1salida:oculto */
#define b0_salida2ac (network_data + 3653)      /* accumulated changes */
#define b0_salida2d (network_data + 3685)       /* delta weights */

/* Black Box Control Functions */


/**** red_1salida ****/

void red_1salida_bb_clear_delays()
{
}/* end red_1salida_bb_clear_delays */

/* red_1salida Forward Propagation */
void red_1salida_propagate_forward()
{

 /* ---------------- red_1salida:oculto ---------------- */
 /* Connection to red_1salida:oculto from $INPUTS */
 BPlvdot(b0_oculto1, b0_input_vector + 0, 36, b0_l1_v, 36, 32);
 /* calculate the transfer function for red_1salida:oculto */
 BPsig1(b0_l1_v, b0_l1_t, 32);

 /* ---------------- red_1salida:salida ---------------- */
 /* Connection to red_1salida:salida from red_1salida:oculto */
 BPlvdot(b0_salida2, b0_l1_v + 0, 32, b0_l0_v, 32, 1);
 /* calculate the transfer function for red_1salida:salida */
 BPsig1(b0_l0_v, b0_l0_t, 1);

 b0fcounter++; /* increment fwd counter */
}/* end red_1salida_ propagate_forward */

/****************** red_1salida Error  ******************/
float red_1salida_calc_error()
{
 float error; 
 /*** red_1salida:salida ***/
 /* Calc diff between output and target output */
 /* Modulate by learning rate times 1/(update_interval) */
 error = BPoutput_error(b0_target_output,b0_l0_v,b0_l0_c,BPlearning_rate*1.000000,1)/1;
 return(error);
} /* end red_1salida_calc_error */

/****************** red_1salida Backward Propagation (Gradient Calc) ******************/
void red_1salida_calc_grad()
{

 b0bcounter++; /* increment bwd counter */

 /* clear errors */
 bzero((char *)b0_l1_c, 32 * sizeof(float));

 /* backprop */

 /* calc deriv of function and multiply by credit */
 BPderiv_sig1(b0_l0_v,b0_l0_c,1,BPfrandom(0.050000));

 /* Accumulate Biases */
 BPaccum_biases(b0_l0_ac,b0_l0_c,1);

 /* Do all connections into red_1salida:salida */

 /* Connection to red_1salida:salida from red_1salida:oculto */

 /* Accumulate weight changes */
 BPaccum_weights_from_hidden(b0_l0_c, b0_l1_v+0,b0_l1_c+0,b0_salida2,b0_salida2ac,  1,32,32);

 /* calc deriv of function and multiply by credit */
 BPderiv_sig1(b0_l1_v,b0_l1_c,32,BPfrandom(0.050000));

 /* Accumulate Biases */
 BPaccum_biases(b0_l1_ac,b0_l1_c,32);

 /* Do all connections into red_1salida:oculto */

 /* Connection to red_1salida:oculto from $INPUTS */

 /* Accumulate weight changes */
 BPaccum_weights_from_input(b0_l1_c, b0_input_vector+0,b0_oculto1ac,  32,36,36);
} /* end red_1salida_calc_grad */

/****************** red_1salida Update Weights ******************/
void red_1salida_update_weights(float scalar)
{

 /* update the weights */

 /* Upate Biases */
 BPupdate_weights(scalar,b0_l0_ac,b0_l0_t,b0_l0_dt,1);
 bzero((char *)b0_l0_ac, 1 * sizeof(float));
 /* Upate Connection from red_1salida:oculto to red_1salida:salida*/
 BPupdate_weights(scalar,b0_salida2ac,b0_salida2,b0_salida2d,32);
 /* Clear Connection from red_1salida:oculto to red_1salida:salida*/
 bzero((char *)b0_salida2ac, 32 * sizeof(float));

 /* Upate Biases */
 BPupdate_weights(scalar,b0_l1_ac,b0_l1_t,b0_l1_dt,32);
 bzero((char *)b0_l1_ac, 32 * sizeof(float));
 /* Upate Connection from $INPUTS to red_1salida:oculto*/
 BPupdate_weights(scalar,b0_oculto1ac,b0_oculto1,b0_oculto1d,1152);
 /* Clear Connection from $INPUTS to red_1salida:oculto*/
 bzero((char *)b0_oculto1ac, 1152 * sizeof(float));
} /* end red_1salida_update_weights */

/* COUNTER CONTROL */
void red_1salida_set_backprop_counter(int x)
{
	b0bcounter = x;
}/* end red_1salida_set_backprop_counter */
int red_1salida_get_backprop_counter()
{
	return(b0bcounter);
}/* end red_1salida_get_backprop_counter */

/* INPUT CONTROL */
void red_1salida_set_input(float *vector)
{
	b0_input_vector = vector;
}/* end red_1salida_set_input */

float *red_1salida_get_input()
{
	return(b0_input_vector);
}/* end red_1salida_get_input */

/* TARGET OUTPUT CONTROL */

float *red_1salida_get_output()
{
	return(b0_l0_v);
}/* end red_1salida_get_output */

void red_1salida_set_target_output(float *vector)
{
	b0_target_output = vector;
}/* end red_1salida_set_target_output */

float *red_1salida_get_target_output()
{
	return(b0_target_output);
}/* end red_1salida_get_target_output */

/* NETWORK FUNCTIONS */

/* red_1salida_error_string: Return string describing error */
char *red_1salida_error_string()
{ 
   return(error_string);
}/* end red_1salida_error_string */

/* red_1salida_set_learning_rate: Set learning rate */
void red_1salida_set_learning_rate(float x)
{ 
   BPlearning_rate = x;
}/* end red_1salida_set_learning_rate */

/* red_1salida_set_inertia: Set inertia*/
void red_1salida_set_inertia(float x)
{ 
   BPinertia = x;
}/* end red_1salida_set_inertia */

/* red_1salida_get_learning_rate: Return the  learning rate */
float red_1salida_get_learning_rate()

{ 
   return(BPlearning_rate);
}/* end red_1salida_get_learning_rate */

/* red_1salida_get_inertia: Return the  inertia*/
float red_1salida_get_inertia()

{ 
   return(BPinertia);
}/* end red_1salida_get_inertia */

static write_header(int fd)
{
 int number;
 int n_extra_bytes = 256;
 char extra_bytes[256];

 /* write header */
 number = 100; /* compiler identifier */
 write(fd, &number, sizeof(int));
 number = 6; /* major version */
 write(fd, &number, sizeof(int));
 number = 0; /* minor version */
 write(fd, &number, sizeof(int));
 /* extra bytes for future use */
 write(fd, extra_bytes, n_extra_bytes);
}/* end write_header */


int red_1salida_dump_network(char *filename)
{
 int number;
 int n_extra_bytes = 256;
 int extra_bytes[256];
 int fd;

 /* open the file */
 fd = creat(filename, 0644);
 if (fd == -1) {
  sprintf(error_string, "\nUnable to open %s.\n", filename);
  return(FERROR);
 }/* end if */
 /* write header */
 write_header(fd);
 number = 1; /* number of black boxes */
 write(fd, &number, sizeof(int));
 number = 4952; /* data size of black box */
 write(fd, &number, sizeof(int));
 /* dump red_1salida */
 write(fd, "red_1salida", 12); /* name */
 number = b0bcounter; /* number of iterations */
 write(fd, &number, sizeof(int));
 number = 2; /* number of layers */
 write(fd, &number, sizeof(int));
 write(fd, "oculto", 7); /* layer name */
 number = 32; /* size of threshold array */
 write(fd, &number, sizeof(int));
 write(fd, b0_l1_t, 32 * sizeof(float)); /* thresholds */
 write(fd, "salida", 7); /* layer name */
 number = 1; /* size of threshold array */
 write(fd, &number, sizeof(int));
 write(fd, b0_l0_t, 1 * sizeof(float)); /* thresholds */
 number = 2; /* number of connection arrays */
 write(fd, &number, sizeof(int));
 write(fd, "oculto", 7); /* to layer name */
 write(fd, "oculto1", 8); /* matrix name */
 number = 1152; /* size of weight array */
 write(fd, &number, sizeof(int));
 write(fd, b0_oculto1, 1152 * sizeof(float)); /* connection */
 write(fd, "salida", 7); /* to layer name */
 write(fd, "salida2", 8); /* matrix name */
 number = 32; /* size of weight array */
 write(fd, &number, sizeof(int));
 write(fd, b0_salida2, 32 * sizeof(float)); /* connection */
 close(fd);
 return(0);
}/* end red_1salida_dump_network */

static TOC_PTR read_header(int fd)
{
 int header_size = 272;
 int number;
 char extra_bytes[256];
 int counter;
 int n_bbs; /* number of black boxes */
 TOC_PTR table_of_contents;
 off_t *address_table; /* used to index bb in a file */

 /* read header */
 read(fd, &number, sizeof(int)); /* compiler type */
 if (number != 100) {
  sprintf(error_string, "\nWarning: This dump file is from another compiler.\n");
 }/* end if */
 read(fd, &number, sizeof(int)); /* major version */
 if (number != 6) {
  sprintf(error_string, "\nWarning: Dump file created with another version (v.%d) of compiler.\n", number);
 }/* end if */
 read(fd, &number, sizeof(int)); /* minor version */
 if (number != 0) {
  sprintf(error_string, "\nWarning: Dump file created with another version of compiler.\n");
 }/* end if */
 /* extra bytes for future use */
 read(fd, extra_bytes, 256);
 read(fd, &n_bbs, sizeof(int));
 /* create a table of black boxes => location in file */
 address_table = (off_t *)am_alloc_mem(n_bbs * sizeof(int));
 header_size += n_bbs * sizeof(int); /* add the address_table size */
 for (counter = 0; counter<n_bbs; counter++) {
  address_table[counter] =  header_size;
  read(fd, &number, sizeof(int));
  header_size += number;
 }/* end for */
 table_of_contents = (TOC_PTR)am_alloc_mem(sizeof(TOC_STRUCT));
 table_of_contents->size = n_bbs;
 table_of_contents->address_table = address_table;
 return(table_of_contents);

}/* end read_header */

static load_black_box(int fd, char *name, char *key, TOC_PTR toc)
{
 int table_size = toc->size;
 off_t *address_table = toc->address_table;
 off_t *end_of_table;
 int n_layers, n_connections, size, counter, error_code, not_found;
 char name_string[100], name_string2[100]; /* for reading names */

 /* record end of address table */
 end_of_table = address_table + table_size;
  /* find key name in the file */
  do {
   if (address_table == end_of_table) {
    sprintf(error_string, "\nUnable to find %s as %s.\n", name, key);
    return(DFERROR);
   }/* end if */
   lseek(fd, *address_table++, 0);
   BPread_string(fd, name_string);
  }while (strcmp(key, name_string) != 0);

 if (strcmp("red_1salida", name) == 0) {
  read(fd, &b0bcounter, sizeof(int)); /* iteration counter */
  read(fd, &n_layers, sizeof(int)); /* n layers of thresholds */
  if (n_layers != 2) {
   sprintf(error_string, "\nError in reading red_1salida\n");
   return(DFERROR);
  }/* end  if */
  /* load all thresholds and feedback weights */
  for(counter = 0; counter<n_layers; counter++) {
   /* read the layer name and data size */
   BPread_string(fd, name_string);
   read(fd, &size, sizeof(int));
   not_found = 1; /* reset flag (0 if read the thresholds) */
   if (error_code = BPread_thresholds(fd,name_string,"oculto",size,32,b0_l1_t,&not_found))
      return(error_code);
   if (error_code = BPread_thresholds(fd,name_string,"salida",size,1,b0_l0_t,&not_found))
      return(error_code);
   if(not_found) {
     sprintf(error_string, "\nUnknown layer name in file: %s\n", name_string);
     return(DFERROR);
   }/* end if not found */
  }/* end for */
  read(fd, &n_connections, sizeof(int));
  for(counter = 0; counter<n_connections; counter++) {
   /* to layer */
   BPread_string(fd, name_string);
   /* from layer */
   BPread_string(fd, name_string2);
   read(fd, &size, sizeof(int));
   not_found = 1; /* reset flag (0 if read the weights) */
   if (error_code = BPread_weights(fd,name_string,"oculto",name_string2,"oculto1",size,1152,b0_oculto1,&not_found))
      return(error_code);
   if (error_code = BPread_weights(fd,name_string,"salida",name_string2,"salida2",size,32,b0_salida2,&not_found))
      return(error_code);
   if(not_found) {
     sprintf(error_string, "\nUnknown connection array in file\n");
     return(DFERROR);
   }/* end if */
  }/* end for */
 return(0);
 }/* end if */
}/* end load_black_box */

static int load_black_box_data(char *name, char *key, char* filename)
{
 int fd;
 TOC_PTR toc;

 /* open the file */
 fd = open(filename, 0);
 if (fd == -1) {
  sprintf(error_string, "\nUnable to open %s.\n", filename);
  return(FERROR);
 }/* end if */
 /* read header */
 toc = read_header(fd);
 if (toc == (TOC_PTR)NULL) return(DFERROR);
 return(load_black_box(fd, name, key, toc));

}/* end load_black_box_data */

int red_1salida_load_network(char *filename)
{
 int fd, error_code;
 TOC_PTR toc; /* table of contents */


 /* open the file */
 fd = open(filename, 0);
 if (fd == -1) {
  sprintf(error_string, "\nUnable to open %s.\n", filename);
  return(FERROR);
 }/* end if */
 /* read header */
 toc = read_header(fd);
 if (toc == (TOC_PTR)NULL) return(DFERROR);
 if (error_code = load_black_box(fd, "red_1salida", "red_1salida", toc))
	return(error_code);
 close(fd);
 return(0);
}/* end red_1salida_load_network */

/* network_forward: Propagate all bb's forward.*/
void red_1salida_network_forward()
{
 /* all black boxes forward! */
  red_1salida_propagate_forward();
}/* end red_1salida_network_forward */

/* red_1salida_network_learn: Propagate all bb's forward then backward */
void red_1salida_network_learn(int iterations, void (*generator)(void) )
{
 float error=0.0;

 while(iterations--) {
  /* execute the generator */
  generator();
  /* all black boxes forward! */
  red_1salida_network_forward();

 /* Calc error on each output black box */
   error += red_1salida_calc_error();

 /* Only calc on significant errors (heuristic to speed learning) */
  if ( error > BPthreshold )  {

 /* Calc grad ient and update on each black box */
    red_1salida_calc_grad();
    red_1salida_update_weights(BPinertia);
  }/* end if error */
  error = 0.0;
 }/* end while iterations */
}/* end red_1salida_network_learn */

void red_1salida_clear_delays()
{
	red_1salida_bb_clear_delays();
}/* end red_1salida_clear_delays */

void red_1salida_set_random_init_seed(long x) { init_seed = x; } 


void red_1salida_set_random_init_range(float x) { init_range = x; } 


int red_1salida_init_network()
{
 int error_code = 0;
 int counter1, counter2, counter3, counter4;
 float *weights;

 BPfrandom_init(init_seed); /* init random number generator */

 /* clear all data */
 bzero((char *)network_data, 3717 * sizeof(float));
 /* init table lookup for sigmoid [-1,1] */
 BPinit_sigmoid_table();

 error_string[0] = '\0'; /* empty string */

 /* init comms_buffer with network information */
 comms_buffer.network_info.aspirin_file = "red_1salida.aspirin";
 comms_buffer.network_info.file = "red_1salida";
 comms_buffer.network_info.temporal = 0;
 comms_buffer.network_info.n_black_boxes = 1;
 comms_buffer.network_info.n_nodes = 33;
 comms_buffer.network_info.n_connections = 1217;
 comms_buffer.network_info.set_learning_rate = red_1salida_set_learning_rate;
 comms_buffer.network_info.set_inertia = red_1salida_set_inertia;
 comms_buffer.network_info.get_learning_rate = red_1salida_get_learning_rate;
 comms_buffer.network_info.get_inertia = red_1salida_get_inertia;
 comms_buffer.network_info.set_random_init_seed = red_1salida_set_random_init_seed;
 comms_buffer.network_info.set_random_init_range = red_1salida_set_random_init_range;
 comms_buffer.network_info.init_network = red_1salida_init_network;
 comms_buffer.network_info.network_forward = red_1salida_network_forward;
 comms_buffer.network_info.network_learn = red_1salida_network_learn;
 comms_buffer.network_info.dump_network = red_1salida_dump_network;
 comms_buffer.network_info.load_network = red_1salida_load_network;
 comms_buffer.network_info.error_string = red_1salida_error_string;
 /* hook up connection buffer */
 comms_buffer.connections = connection_buffer;

 /* initializing red_1salida */
 /* init for red_1salida:salida */
  /* init thresholds (biases) */
 for(counter1 = 0; counter1<1; counter1++) {
  b0_l0_t[counter1] = BPfrandom(2.0 * init_range) - init_range;
 }/* end for counter1 */
 /* init weights for red_1salida:salida */
 /* initialize w/small random weights */
 for(counter1 = 0; counter1<32; counter1++) {
  b0_salida2[counter1] = BPfrandom(2.0 * init_range) - init_range;
 }/* end for */
 /* init for red_1salida:oculto */
  /* init thresholds (biases) */
 for(counter1 = 0; counter1<32; counter1++) {
  b0_l1_t[counter1] = BPfrandom(2.0 * init_range) - init_range;
 }/* end for counter1 */
 /* init weights for red_1salida:oculto */
 /* initialize w/small random weights */
 for(counter1 = 0; counter1<1152; counter1++) {
  b0_oculto1[counter1] = BPfrandom(2.0 * init_range) - init_range;
 }/* end for */
 return(error_code);
}/* end red_1salida_init_network */

/* red_1salida_query_network   This function returns a pointer to a    */
/*                 layer buffer specified by the integer    */
/*                 pair of arguments. The first integer     */
/*                 is the black box index and the second    */
/*                 is the layer index. If no layer has      */
/*                 these indices then (LB_PTR)NULL is       */
/*                 returned. There is only ONE LAYER BUFFER,*/
/*                 so COPY!! the data you need for display. */
/*                 To change the network state alter the    */
/*                 data in the layer buffer.                */
LB_PTR red_1salida_query_network(int bb_index, int layer_index)
{
 /* look up the black box first, then the layer in the black
    box, if either index does not exist return (LB_PTR)NULL.
 */
 /* Black Box red_1salida */
 if(bb_index ==  0)
  {
    comms_buffer.black_box_info.bb_index = 0;
    comms_buffer.black_box_info.bb_name = "red_1salida";
    comms_buffer.black_box_info.dynamic = 1;
    comms_buffer.black_box_info.efferent = 1;
    comms_buffer.black_box_info.n_layers = 2;
    comms_buffer.black_box_info.n_inputs = 36;
    comms_buffer.black_box_info.n_bb_inputs = 0;
    comms_buffer.black_box_info.n_bb_outputs = 0;
    comms_buffer.black_box_info.inputs_xdim = 36;
    comms_buffer.black_box_info.inputs_ydim = 1;
    comms_buffer.black_box_info.inputs_delay = 0;
    comms_buffer.black_box_info.output_layer_index = 0;
    comms_buffer.black_box_info.forward_prop = red_1salida_propagate_forward;
    comms_buffer.black_box_info.set_backprop_counter = red_1salida_set_backprop_counter;
    comms_buffer.black_box_info.get_backprop_counter = red_1salida_get_backprop_counter;
    comms_buffer.black_box_info.calc_grad = red_1salida_calc_grad;
    comms_buffer.black_box_info.update_weights = red_1salida_update_weights;
    comms_buffer.black_box_info.set_target_output = red_1salida_set_target_output;
    comms_buffer.black_box_info.get_target_output = red_1salida_get_target_output;
    comms_buffer.black_box_info.get_output = red_1salida_get_output;
    comms_buffer.black_box_info.set_input = red_1salida_set_input;
    comms_buffer.black_box_info.get_input = red_1salida_get_input;
   /* Layer red_1salida:oculto */
   if (layer_index == 1)
   {
     comms_buffer.layer_index = 1;
     comms_buffer.layer_type = 0;
     comms_buffer.layer_name = "red_1salida:oculto";
     comms_buffer.n_nodes = 32;
     comms_buffer.delay = 0;
     comms_buffer.xdim = 32;
     comms_buffer.ydim = 1;
     comms_buffer.values = b0_l1_v;
     comms_buffer.thresholds = b0_l1_t;
     comms_buffer.delta_thresholds = b0_l1_dt;
     comms_buffer.credit = b0_l1_c;
     comms_buffer.order = 0;
     (comms_buffer.connections)[0].type = 0;
     (comms_buffer.connections)[0].order = 1;
     (comms_buffer.connections)[0].shared = 0;
     (comms_buffer.connections)[0].size = 1152;
     (comms_buffer.connections)[0].delay = 0;
     (comms_buffer.connections)[0].from_layer_name = "$INPUTS";
     (comms_buffer.connections)[0].tess_xdim = 36;
     (comms_buffer.connections)[0].tess_ydim = 1;
     (comms_buffer.connections)[0].tess_xoverlap = 36;
     (comms_buffer.connections)[0].tess_yoverlap = 0;
     (comms_buffer.connections)[0].tess_xoffset = 0;
     (comms_buffer.connections)[0].tess_yoffset = 0;
     (comms_buffer.connections)[0].from_bb_index = 0;
     (comms_buffer.connections)[0].from_layer_index = -1; /* $INPUTS */
     (comms_buffer.connections)[0].from_size = 36;
     (comms_buffer.connections)[0].from_xdim = 36;
     (comms_buffer.connections)[0].from_ydim = 1;
     (comms_buffer.connections)[0].weights = b0_oculto1;
     (comms_buffer.connections)[0].delta_weights = b0_oculto1d;
     comms_buffer.n_inputs = 1;
     comms_buffer.n_outputs = 1;
     return(&comms_buffer);/* success!! */
    }/* end if for Layer red_1salida:oculto (number 1) */
   /* Layer red_1salida:salida */
   if (layer_index == 0)
   {
     comms_buffer.layer_index = 0;
     comms_buffer.layer_type = 0;
     comms_buffer.layer_name = "red_1salida:salida";
     comms_buffer.n_nodes = 1;
     comms_buffer.delay = 0;
     comms_buffer.xdim = 1;
     comms_buffer.ydim = 1;
     comms_buffer.values = b0_l0_v;
     comms_buffer.thresholds = b0_l0_t;
     comms_buffer.delta_thresholds = b0_l0_dt;
     comms_buffer.credit = b0_l0_c;
     comms_buffer.order = 0;
     (comms_buffer.connections)[0].type = 0;
     (comms_buffer.connections)[0].order = 1;
     (comms_buffer.connections)[0].shared = 0;
     (comms_buffer.connections)[0].size = 32;
     (comms_buffer.connections)[0].delay = 0;
     (comms_buffer.connections)[0].from_layer_name = "red_1salida:oculto";
     (comms_buffer.connections)[0].tess_xdim = 32;
     (comms_buffer.connections)[0].tess_ydim = 1;
     (comms_buffer.connections)[0].tess_xoverlap = 32;
     (comms_buffer.connections)[0].tess_yoverlap = 0;
     (comms_buffer.connections)[0].tess_xoffset = 0;
     (comms_buffer.connections)[0].tess_yoffset = 0;
     (comms_buffer.connections)[0].from_bb_index = 0;
     (comms_buffer.connections)[0].from_layer_index = 1;
     (comms_buffer.connections)[0].from_size = 32;
     (comms_buffer.connections)[0].from_xdim = 32;
     (comms_buffer.connections)[0].from_ydim = 1;
     (comms_buffer.connections)[0].weights = b0_salida2;
     (comms_buffer.connections)[0].delta_weights = b0_salida2d;
     comms_buffer.n_inputs = 1;
     comms_buffer.n_outputs = 0;
     return(&comms_buffer);/* success!! */
    }/* end if for Layer red_1salida:salida (number 0) */
  return((LB_PTR)NULL);
  }/* end if for black box red_1salida (number: 0) */
 return((LB_PTR)NULL);
}/* end red_1salida_query_network */


void red_1salida_ascii_dump_network(int formatted)
{
 int counter1, counter2, counter3, counter4;
 float *weights;

  if (! formatted) printf("\n\n#");

 if (formatted) printf("\n\nBlackBox: red_1salida");

 if (formatted) printf("\nIterations: %d", b0bcounter);

 else printf("\n%d", b0bcounter);

 if (formatted) printf("\nLayer: red_1salida:salida");
 /* print for red_1salida:salida */
 for(counter4 = 0; counter4<1; counter4++) {
  for(counter3 = 0; counter3<1; counter3++) {

    if (formatted) printf("\nNode[%d,%d]:",counter3,counter4);

    if (formatted)
      printf("\n   Bias=%.12f", b0_l0_t[counter3+(1*counter4)]);
      else
      printf("\n%.12f", b0_l0_t[counter3+(1*counter4)]);
  }/* end for counter3 */
 }/* end for counter4 */

 if (formatted) printf("\nConnection from red_1salida:oculto:");
 for(counter4 = 0; counter4<1; counter4++) {
  for(counter3 = 0; counter3<1; counter3++) {

 if (formatted) printf("\nNode[%d,%d]:",counter3,counter4);
   weights = b0_salida2 + (counter3 * 32) + (counter4 * 32);
   for(counter1 = 0; counter1<1; counter1++) {
    if (formatted) printf("\n");
    for(counter2 = 0; counter2<32; counter2++) {
       if (formatted)
         printf("%.12f ", *weights++);
       else
         printf("\n%.12f ", *weights++);
    }/* end for */
   }/* end for */
  }/* end for */
 }/* end for */

 if (formatted) printf("\nLayer: red_1salida:oculto");
 /* print for red_1salida:oculto */
 for(counter4 = 0; counter4<1; counter4++) {
  for(counter3 = 0; counter3<32; counter3++) {

    if (formatted) printf("\nNode[%d,%d]:",counter3,counter4);

    if (formatted)
      printf("\n   Bias=%.12f", b0_l1_t[counter3+(32*counter4)]);
      else
      printf("\n%.12f", b0_l1_t[counter3+(32*counter4)]);
  }/* end for counter3 */
 }/* end for counter4 */

 if (formatted) printf("\nConnection from $INPUTS:");
 for(counter4 = 0; counter4<1; counter4++) {
  for(counter3 = 0; counter3<32; counter3++) {

 if (formatted) printf("\nNode[%d,%d]:",counter3,counter4);
   weights = b0_oculto1 + (counter3 * 36) + (counter4 * 1152);
   for(counter1 = 0; counter1<1; counter1++) {
    if (formatted) printf("\n");
    for(counter2 = 0; counter2<36; counter2++) {
       if (formatted)
         printf("%.12f ", *weights++);
       else
         printf("\n%.12f ", *weights++);
    }/* end for */
    weights += 1116;
   }/* end for */
  }/* end for */
 }/* end for */
  if (! formatted) printf("\n\n#");
  printf("\n");
}/* end red_1salida_ascii_dump_network */

void red_1salida_load_ascii_network()
{
 int counter1, counter2, counter3, counter4;
 float *weights;


 {
    char c;
    do {
      am_fread(&c,sizeof(char),1,stdin);
    } while( c != '#' );

 }

 scanf("\n%d", &b0bcounter);
 /* read for red_1salida:salida */
 for(counter4 = 0; counter4<1; counter4++) {
  for(counter3 = 0; counter3<1; counter3++) {

    scanf("%f", &b0_l0_t[counter3+(1*counter4)]);
  }/* end for counter3 */
 }/* end for counter4 */
 for(counter4 = 0; counter4<1; counter4++) {
  for(counter3 = 0; counter3<1; counter3++) {
   weights = b0_salida2 + (counter3 * 32) + (counter4 * 32);
   for(counter1 = 0; counter1<1; counter1++) {
    for(counter2 = 0; counter2<32; counter2++) {
       scanf("%f", weights++);
    }/* end for */
   }/* end for */
  }/* end for */
 }/* end for */
 /* read for red_1salida:oculto */
 for(counter4 = 0; counter4<1; counter4++) {
  for(counter3 = 0; counter3<32; counter3++) {

    scanf("%f", &b0_l1_t[counter3+(32*counter4)]);
  }/* end for counter3 */
 }/* end for counter4 */
 for(counter4 = 0; counter4<1; counter4++) {
  for(counter3 = 0; counter3<32; counter3++) {
   weights = b0_oculto1 + (counter3 * 36) + (counter4 * 1152);
   for(counter1 = 0; counter1<1; counter1++) {
    for(counter2 = 0; counter2<36; counter2++) {
       scanf("%f", weights++);
    }/* end for */
    weights += 1116;
   }/* end for */
  }/* end for */
 }/* end for */

 {
    int  code;
    char c;
    do {
      code = am_fread(&c,sizeof(char),1,stdin);
    } while( code );

 }
}/* end red_1salida_load_ascii_network */

/* network_initialize: Init simulation, optionaly load dump.*/
void network_initialize(char *filename, int verbose)
{
 /* initialize the simulator */
 if (red_1salida_init_network()) {
   fprintf(stderr, "%s", red_1salida_error_string());
   am_exit( EXIT_FAILURE );
 }/* end if init */
 /* load dump file? */
 if (filename != (char *)NULL) {
  if (verbose) printf("\nLoading Saved Network: %s", filename);
  if (red_1salida_load_network(filename)) {
    fprintf(stderr, "%s", red_1salida_error_string());
    am_exit( EXIT_FAILURE );
  }/* end if load_network */
 }/* end if filename */

 if (verbose) {
  printf("\n\nBackpropagation Learning");
  printf("\n\nBlack Boxes:\n");
  printf("\nred_1salida (Saved at Backprop Iteration %d)", red_1salida_get_backprop_counter());
 }/* end if verbose */
}/* end network_initialize */

/* network_forward: Propagate all bb's forward.*/
void network_forward(int iterations, void (*generator)(void) )
{
  while (iterations--) {
   generator();
   red_1salida_network_forward();
  }/* end while */
}/* end network_forward */

/* network_learn: Propagate all bb's forward then backward */
void network_learn(int iterations, void (*generator)(void) )
{
     red_1salida_network_learn(iterations, generator);
}/* end network_learn */

void network_clear_delays()
{
  red_1salida_clear_delays();
}/* end network_clear_delays */

void network_dump(char *filename)
{
 if (red_1salida_dump_network(filename)) {
   fprintf(stderr, "%s", red_1salida_error_string());
}/* end if dump*/
}/* end network_dump */

void network_load(char *filename)
{
 if (red_1salida_load_network(filename)) {
   fprintf(stderr, "%s", red_1salida_error_string());
}/* end if load*/
}/* end network_load */

void network_ascii_dump(int formatted)
{
 red_1salida_ascii_dump_network(formatted);
}/* end network_ascii_dump */

void network_load_ascii()
{
 red_1salida_load_ascii_network();
}/* end network_load_ascii */

LB_PTR network_query(int bbindex, int layerindex)
{
 return(red_1salida_query_network(bbindex, layerindex));
}/* end network_query */

/* network_forward_print: print outputs/target for each bb */
void network_forward_print(int iterations, void (*generator)(void))
{
   int icounter,counter;
   float *ptr;

  for(icounter=0;icounter<iterations;icounter++) {
   generator();
   red_1salida_network_forward();
   printf("\nIteration: %d", icounter);
   printf("\nblack box red_1salida:");
   ptr = red_1salida_get_output();
   counter = 1;
   printf("\n\tOutputs:");
   while(counter--)
     printf(" %f", *ptr++);
   ptr = red_1salida_get_target_output();
   counter = 1;
   printf("\n\tTargets:");
   while(counter--)
     printf(" %f", *ptr++);
  }/* end while */
}/* end network_forward_print */

/* network_forward_pdpfa: calc pd, pfa, pfn */
void network_forward_pdpfa(int iterations, void (*generator)(void), float threshold)
{
   int counter;
   float *ptr1, *ptr2;
  int total = iterations;
  float red_1salida_sum;
  int red_1salida_detections = 0;
  double red_1salidaMin[1];
  double red_1salidaMax[1];
  float red_1salidaMean[1];
  float red_1salidaSquare[1];

  if (!iterations) return;
  threshold *= threshold;
  for(counter=0;counter<1;counter++) { /* init */
     red_1salidaMin[counter] = AM_HUGE_VAL;
     red_1salidaMax[counter] = -AM_HUGE_VAL;
     red_1salidaMean[counter] = 0.0;
     red_1salidaSquare[counter] = 0.0;
  }/* end for */
  while (iterations--) {
   generator();
   red_1salida_network_forward();
   red_1salida_sum = 0.0;
   ptr1 = red_1salida_get_output();
   ptr2 = red_1salida_get_target_output();
   for(counter=0;counter<1;counter++) { /* calc sum of squared diffs */
     float val, target, diff;

     val = *ptr1++;
     target = *ptr2++;
     red_1salidaMean[counter] += val;
     red_1salidaSquare[counter] += val * val;
     if (val < red_1salidaMin[counter]) red_1salidaMin[counter] = val;
     if (val > red_1salidaMax[counter]) red_1salidaMax[counter] = val;
     diff = val - target;
     red_1salida_sum += diff * diff;
   }/* end for */
   /* count */
   if (red_1salida_sum < threshold) red_1salida_detections++;
  }/* end while */

  printf("\n\nred_1salida:");
  printf("\nMaxima: ");
  for(counter=0;counter<1;counter++) { /* calc max */
   printf("%f ",red_1salidaMax[counter]);
  }/* end for */
  printf("\nMinima: ");
  for(counter=0;counter<1;counter++) { /* calc min */
   printf("%f ",red_1salidaMin[counter]);
  }/* end for */
  printf("\nMeans: ");
  for(counter=0;counter<1;counter++) { /* calc mean */
     red_1salidaMean[counter] /= (float)total;
     printf("%f ",red_1salidaMean[counter]);
  }/* end for */
  printf("\nVariances: ");
  for(counter=0;counter<1;counter++) { /* calc variance */
     red_1salidaSquare[counter] /= (float)total;
     printf("%f ",red_1salidaSquare[counter] - (red_1salidaMean[counter] * red_1salidaMean[counter]));
  }/* end for */
  /* print stats */
  printf("\n\nPd = %d/%d = %f",red_1salida_detections, total, (float)red_1salida_detections/total);
  printf("\nPfa = %d/%d = %f", total - red_1salida_detections, total, (float)(total - red_1salida_detections)/total);
}/* end network_forward_pdpfa */

/* end of red_1salida.c */

