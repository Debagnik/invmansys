#include "../include/storage.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int storage_load_db(const char *filepath, InventoryDb *db) {
    if (!filepath || !db) return -1;
    
    FILE *f = fopen(filepath, "rb");
    if (!f) {
        return 0;
    }

    DebagnikHeader header;
    if (fread(&header, sizeof(header), 1, f) != 1) {
        fclose(f);
        return -2; 
    }

    if (header.magic != DEBAGNIK_MAGIC) {
        fclose(f);
        return -3; 
    }

    if (header.record_count > 0) {
        if (db->capacity < header.record_count) {
            StockItem *new_items = realloc(db->items, header.record_count * sizeof(StockItem));
            if (!new_items) {
                fclose(f);
                return -4; 
            }
            db->items = new_items;
            db->capacity = header.record_count;
        }

        size_t read = fread(db->items, sizeof(StockItem), header.record_count, f);
        if (read != header.record_count) {
            fclose(f);
            return -5; 
        }
        db->count = header.record_count;
    }

    fclose(f);
    return 0;
}

int storage_save_db(const char *filepath, const InventoryDb *db) {
    if (!filepath || !db) return -1;
    
    FILE *f = fopen(filepath, "wb");
    if (!f) return -2;

    DebagnikHeader header;
    memset(&header, 0, sizeof(header));
    header.magic = DEBAGNIK_MAGIC;
    header.version = 2;
    header.record_size = sizeof(StockItem);
    header.record_count = (uint32_t)db->count;

    if (fwrite(&header, sizeof(header), 1, f) != 1) {
        fclose(f);
        return -3;
    }

    if (db->count > 0) {
        if (fwrite(db->items, sizeof(StockItem), db->count, f) != db->count) {
            fclose(f);
            return -4;
        }
    }

    fclose(f);
    return 0;
}

int storage_backup_db(const char *db_filepath, const char *backup_filepath) {
    if (!db_filepath || !backup_filepath) return -1;
    
    FILE *src = fopen(db_filepath, "rb");
    if (!src) return -2; 
    
    FILE *dst = fopen(backup_filepath, "wb");
    if (!dst) {
        fclose(src);
        return -3;
    }

    char buf[4096];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), src)) > 0) {
        if (fwrite(buf, 1, n, dst) != n) {
            fclose(src);
            fclose(dst);
            return -4;
        }
    }

    fclose(src);
    fclose(dst);
    return 0;
}
