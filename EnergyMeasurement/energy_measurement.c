#include "energy_measurement.h"

void init_energy_measurement(energy_measurement_t * e_msrnt, energy_plugin_t * plug)
{
  e_msrnt->total_time_elapsed = 0.0;
  e_msrnt->total_energy_consumed = 0.0;
  e_msrnt->last_energy_measured = 0.0;

  e_msrnt->plugin = plug;
}

void trigger_energy_measurement(energy_measurement_t * e_msrnt)
{
  struct timespec current_time;
  clock_gettime(CLOCK_REALTIME, &current_time);
  e_msrnt->total_time_elapsed = (current_time.tv_sec - e_msrnt->current_time.tv_sec) * 1e3 + (current_time.tv_nsec - e_msrnt->current_time.tv_nsec) / 1e6;    // in milliseconds

  double current_energy = e_msrnt->plugin->f(e_msrnt->plugin->data);
  e_msrnt->total_energy_consumed = current_energy - e_msrnt->last_energy_measured;
}

void start_energy_measurement(energy_measurement_t * e_msrnt)
{
  clock_gettime(CLOCK_REALTIME, &e_msrnt->current_time);
  e_msrnt->last_energy_measured = e_msrnt->plugin->f(e_msrnt->plugin->data);
}

void stop_energy_measurement(energy_measurement_t * e_msrnt)
{
  struct timespec current_time;
  clock_gettime(CLOCK_REALTIME, &current_time);
  e_msrnt->total_time_elapsed += (current_time.tv_sec - e_msrnt->current_time.tv_sec) * 1e3 + (current_time.tv_nsec - e_msrnt->current_time.tv_nsec) / 1e6;    // in millicroseconds

  double current_energy = e_msrnt->plugin->f(e_msrnt->plugin->data);
  e_msrnt->total_energy_consumed += current_energy - e_msrnt->last_energy_measured;

  e_msrnt->current_time = current_time;
  e_msrnt->last_energy_measured = current_energy;
}
