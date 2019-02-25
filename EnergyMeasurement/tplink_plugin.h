#ifndef __TPLINK_PLUGIN__H
#define __TPLINK_PLUGIN__H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <time.h>

#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <byteswap.h>


#include "energy_plugin.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
  double energy_consumed;
  double last_measure_timestamp;
  double delta_time_measures;
  int is_running;
  const char * hostname;
  regex_t reg;
  pthread_t measure_thread;
} tplink_measurement_t;

int init_tplink_plugin(tplink_measurement_t * tplink_msrnt, const char * hostname, double delta_time_measures);

double get_current_energy_tplink(energy_data_t data);

#ifdef __cplusplus
}
#endif


#endif
