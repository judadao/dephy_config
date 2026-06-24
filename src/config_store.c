#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "dephy_config/config_store.h"

#ifndef __ZEPHYR__

#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

static const char *store_dir(void)
{
    const char *dir = getenv("DEPHY_CONFIG_DIR");
    return (dir && dir[0]) ? dir : "/tmp/dephy_config";
}

static int key_path(dephy_config_key_t key, char *out, size_t out_cap)
{
    int n;

    if (!out || out_cap == 0) {
        return -EINVAL;
    }
    n = snprintf(out, out_cap, "%s/%u.bin", store_dir(), (unsigned)key);
    if (n < 0 || (size_t)n >= out_cap) {
        return -ENAMETOOLONG;
    }
    return 0;
}

int dephy_config_store_init(void)
{
    if (mkdir(store_dir(), 0775) == 0 || errno == EEXIST) {
        return 0;
    }
    return -EIO;
}

int dephy_config_store_load(dephy_config_key_t key, void *data, size_t size)
{
    char path[256];
    FILE *f;
    size_t n;

    if (!data || size == 0) {
        return -EINVAL;
    }
    if (key_path(key, path, sizeof(path)) != 0) {
        return -ENAMETOOLONG;
    }
    f = fopen(path, "rb");
    if (!f) {
        return -ENOENT;
    }
    n = fread(data, 1, size, f);
    fclose(f);
    return n == size ? 0 : -EIO;
}

int dephy_config_store_save(dephy_config_key_t key, const void *data, size_t size)
{
    char path[256];
    FILE *f;
    size_t n;

    if (!data || size == 0) {
        return -EINVAL;
    }
    if (dephy_config_store_init() != 0 ||
        key_path(key, path, sizeof(path)) != 0) {
        return -EIO;
    }
    f = fopen(path, "wb");
    if (!f) {
        return -EIO;
    }
    n = fwrite(data, 1, size, f);
    fclose(f);
    return n == size ? 0 : -EIO;
}

int dephy_config_store_delete(dephy_config_key_t key)
{
    char path[256];

    if (key_path(key, path, sizeof(path)) != 0) {
        return -ENAMETOOLONG;
    }
    return remove(path) == 0 || errno == ENOENT ? 0 : -EIO;
}

#else

#include <zephyr/device.h>
#include <zephyr/fs/nvs.h>
#include <zephyr/kernel.h>
#include <zephyr/storage/flash_map.h>

#define DEPHY_CONFIG_NVS_SECTOR_SIZE  DT_PROP(DT_CHOSEN(zephyr_flash), erase_block_size)
#define DEPHY_CONFIG_NVS_SECTOR_COUNT 2
#define DEPHY_CONFIG_NVS_PARTITION    storage_partition

static struct nvs_fs nvs;
static int nvs_ready;
static K_MUTEX_DEFINE(nvs_init_lock);

int dephy_config_store_init(void)
{
    int rc;

    k_mutex_lock(&nvs_init_lock, K_FOREVER);
    if (nvs_ready) {
        k_mutex_unlock(&nvs_init_lock);
        return 0;
    }
    nvs.flash_device = FIXED_PARTITION_DEVICE(DEPHY_CONFIG_NVS_PARTITION);
    if (!device_is_ready(nvs.flash_device)) {
        k_mutex_unlock(&nvs_init_lock);
        return -ENODEV;
    }
    nvs.sector_size = DEPHY_CONFIG_NVS_SECTOR_SIZE;
    nvs.sector_count = DEPHY_CONFIG_NVS_SECTOR_COUNT;
    nvs.offset = FIXED_PARTITION_OFFSET(DEPHY_CONFIG_NVS_PARTITION);
    rc = nvs_mount(&nvs);
    if (rc == 0) {
        nvs_ready = 1;
    }
    k_mutex_unlock(&nvs_init_lock);
    return rc;
}

int dephy_config_store_load(dephy_config_key_t key, void *data, size_t size)
{
    ssize_t n;

    if (!data || size == 0) {
        return -EINVAL;
    }
    if (dephy_config_store_init() != 0) {
        return -ENODEV;
    }
    n = nvs_read(&nvs, key, data, size);
    return n == (ssize_t)size ? 0 : -ENOENT;
}

int dephy_config_store_save(dephy_config_key_t key, const void *data, size_t size)
{
    ssize_t n;

    if (!data || size == 0) {
        return -EINVAL;
    }
    if (dephy_config_store_init() != 0) {
        return -ENODEV;
    }
    n = nvs_write(&nvs, key, data, size);
    return n < 0 ? (int)n : 0;
}

int dephy_config_store_delete(dephy_config_key_t key)
{
    if (dephy_config_store_init() != 0) {
        return -ENODEV;
    }
    return nvs_delete(&nvs, key);
}

#endif
