#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dephy_config/config_store.h"

typedef struct {
    char name[16];
    int value;
} test_config_t;

int main(void)
{
    test_config_t in = { .name = "node-a", .value = 42 };
    test_config_t out;

    setenv("DEPHY_CONFIG_DIR", "/tmp/dephy_config_smoke", 1);
    if (dephy_config_store_init() != 0) {
        return 1;
    }
    (void)dephy_config_store_delete(100);
    if (dephy_config_store_save(100, &in, sizeof(in)) != 0) {
        return 1;
    }
    memset(&out, 0, sizeof(out));
    if (dephy_config_store_load(100, &out, sizeof(out)) != 0) {
        return 1;
    }
    if (strcmp(out.name, "node-a") != 0 || out.value != 42) {
        fprintf(stderr, "unexpected config data\n");
        return 1;
    }
    if (dephy_config_store_delete(100) != 0) {
        return 1;
    }
    printf("dephy_config smoke passed\n");
    return 0;
}
