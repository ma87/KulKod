#ifndef __ENERGY_MEASUREMENT__H__
#define __ENERGY_MEASUREMENT__H__

#include <time.h>
#include "rapl.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
  struct timespec current_time;
  double total_time_elapsed;
  double total_energy_consumed;
  double last_energy_measured;
  rapl_measurement_t * rapl_measurement;
} energy_measurement_t;

void init_energy_measurement(energy_measurement_t * e_msrnt, rapl_measurement_t * e_rapl);

void trigger_energy_measurement(energy_measurement_t * e_msrnt);

void start_energy_measurement(energy_measurement_t * e_msrnt);

void stop_energy_measurement(energy_measurement_t * e_msrnt);

#ifdef __cplusplus
}
#endif

#endif
