#ifndef STORAGE_H
#define STORAGE_H

#include "inventory.h"
#include <stdint.h>

#define DEBAGNIK_MAGIC 0xDEBA6002

typedef struct {
    uint32_t magic;
    uint32_t version;
    uint32_t record_size;
    uint32_t record_count;
    uint8_t reserved[16];
} DebagnikHeader;

int storage_load_db(const char *filepath, InventoryDb *db);
int storage_save_db(const char *filepath, const InventoryDb *db);
int storage_backup_db(const char *db_filepath, const char *backup_filepath);

#endif // STORAGE_H
