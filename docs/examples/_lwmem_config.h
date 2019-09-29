#ifndef LWMEM_HDR_CONFIG_H
#define LWMEM_HDR_CONFIG_H

/* Rename this file to "lwmem_config.h" for your application */

/* Enable operating system support */
#define LWMEM_CFG_OS                        1

/* After user configuration, call default config to merge config together */
#include "lwmem/lwmem_config_default.h"

#endif /* LWMEM_HDR_CONFIG_H */