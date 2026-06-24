#ifndef DEPHY_CONFIG_CONFIG_STORE_H
#define DEPHY_CONFIG_CONFIG_STORE_H

#include <stddef.h>
#include <stdint.h>

typedef uint16_t dephy_config_key_t;

int dephy_config_store_init(void);
int dephy_config_store_load(dephy_config_key_t key, void *data, size_t size);
int dephy_config_store_save(dephy_config_key_t key, const void *data, size_t size);
int dephy_config_store_delete(dephy_config_key_t key);

#endif /* DEPHY_CONFIG_CONFIG_STORE_H */
