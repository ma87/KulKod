#include "energy_measurement.h"

void init_energy_measurement(energy_measurement_t * e_msrnt, rapl_measurement_t * e_rapl)
{
  e_msrnt->total_time_elapsed = 0.0;
  e_msrnt->total_energy_consumed = 0.0;
  e_msrnt->last_energy_measured = 0.0;

  e_msrnt->rapl_measurement = e_rapl;
}

void trigger_energy_measurement(energy_measurement_t * e_msrnt)
{
  struct timespec current_time;
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &current_time);
  e_msrnt->total_time_elapsed = (current_time.tv_sec - e_msrnt->current_time.tv_sec) * 1e6 + (current_time.tv_nsec - e_msrnt->current_time.tv_nsec) / 1e3;    // in microseconds

  double current_energy = get_current_energy(e_msrnt->rapl_measurement);
  e_msrnt->total_energy_consumed = current_energy - e_msrnt->last_energy_measured;
}

void start_energy_measurement(energy_measurement_t * e_msrnt)
{
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &e_msrnt->current_time);
  e_msrnt->last_energy_measured = get_current_energy(e_msrnt->rapl_measurement);
}

void stop_energy_measurement(energy_measurement_t * e_msrnt)
{
  struct timespec current_time;
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &current_time);
  e_msrnt->total_time_elapsed += (current_time.tv_sec - e_msrnt->current_time.tv_sec) * 1e6 + (current_time.tv_nsec - e_msrnt->current_time.tv_nsec) / 1e3;    // in microseconds

  double current_energy = get_current_energy(e_msrnt->rapl_measurement);
  e_msrnt->total_energy_consumed += current_energy - e_msrnt->last_energy_measured;

  e_msrnt->current_time = current_time;
  e_msrnt->last_energy_measured = current_energy;
}
