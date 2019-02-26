#ifndef __ENERGY_PLUGIN__H__
#define __ENERGY_PLUGIN__H__

#define NUMBER_PLUGINS 2

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    RAPL,
    TPLINK_HS110
} TYPE_PLUGIN;

typedef void * energy_data_t;
typedef double (*get_energy_func)(energy_data_t data);
typedef struct
{
    get_energy_func f;
    energy_data_t data;
    TYPE_PLUGIN p;
} energy_plugin_t;

#ifdef __cplusplus
}
#endif

#endif
