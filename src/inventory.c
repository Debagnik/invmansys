#include "../include/inventory.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define INITIAL_CAPACITY 16

InventoryDb* inv_db_create(void) {
    InventoryDb *db = malloc(sizeof(InventoryDb));
    if (!db) return NULL;
    db->capacity = INITIAL_CAPACITY;
    db->count = 0;
    db->items = malloc(db->capacity * sizeof(StockItem));
    if (!db->items) {
        free(db);
        return NULL;
    }
    return db;
}

void inv_db_free(InventoryDb *db) {
    if (db) {
        if (db->items) {
            free(db->items);
        }
        free(db);
    }
}

static int find_index(const InventoryDb *db, const char *code) {
    for (size_t i = 0; i < db->count; i++) {
        if (strcmp(db->items[i].itemcode, code) == 0) {
            return (int)i;
        }
    }
    return -1;
}

int inv_add_item(InventoryDb *db, const StockItem *item) {
    if (!db || !item) return -1;
    if (find_index(db, item->itemcode) >= 0) {
        return -2; // Duplicate code
    }

    if (db->count >= db->capacity) {
        size_t new_cap = db->capacity * 2;
        StockItem *new_items = realloc(db->items, new_cap * sizeof(StockItem));
        if (!new_items) return -1; // OOM
        db->items = new_items;
        db->capacity = new_cap;
    }

    db->items[db->count++] = *item;
    return 0; // Success
}

int inv_update_item(InventoryDb *db, const StockItem *item) {
    if (!db || !item) return -1;
    int idx = find_index(db, item->itemcode);
    if (idx < 0) return -2; // Not found
    
    db->items[idx] = *item;
    return 0;
}

int inv_find_item(const InventoryDb *db, const char *code, StockItem *out_item) {
    if (!db || !code || !out_item) return -1;
    int idx = find_index(db, code);
    if (idx < 0) return -2; // Not found

    *out_item = db->items[idx];
    return 0;
}

int inv_issue_stock(InventoryDb *db, const char *code, float qty, float *out_total_cost) {
    if (!db || !code) return -1;
    int idx = find_index(db, code);
    if (idx < 0) return -2; // Not found

    StockItem *item = &db->items[idx];
    if (item->itemqty < qty) {
        return -3; // Insufficient overall stock
    }
    if ((item->itemqty - qty) < item->minqty) {
        return -4; // Hits reorder level constraint
    }

    item->itemqty -= qty;
    if (out_total_cost) {
        *out_total_cost = qty * item->itemrate;
    }
    return 0;
}

double inv_calculate_total_investment(const InventoryDb *db) {
    if (!db) return 0.0;
    double total = 0.0;
    for (size_t i = 0; i < db->count; i++) {
        total += (double)db->items[i].itemqty * (double)db->items[i].itemrate;
    }
    return total;
}
