# dephy_config

Reusable key/blob config store for Dephy products.

The module provides a small persistence API backed by Zephyr NVS on hardware
and files on POSIX test builds. Product code owns schema, defaults, and
validation; this module only stores fixed-size binary records by key.

## API

```c
#include <dephy_config/config_store.h>

dephy_config_store_init();
dephy_config_store_save(100, &settings, sizeof(settings));
dephy_config_store_load(100, &settings, sizeof(settings));
```

On POSIX, set `DEPHY_CONFIG_DIR` to isolate test storage.
