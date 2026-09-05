#ifndef INVENTORY_H
#define INVENTORY_H

#include <stddef.h>

#define MAX_CODE_LEN 12
#define MAX_NAME_LEN 64
#define MAX_UNIT_LEN 16

typedef struct {
    char itemcode[MAX_CODE_LEN];
    char itemname[MAX_NAME_LEN];
    float itemrate;
    float itemqty;
    int minqty;
    char unit[MAX_UNIT_LEN];
} StockItem;

typedef struct {
    StockItem *items;
    size_t count;
    size_t capacity;
} InventoryDb;

InventoryDb* inv_db_create(void);
void inv_db_free(InventoryDb *db);

int inv_add_item(InventoryDb *db, const StockItem *item);
int inv_update_item(InventoryDb *db, const StockItem *item);
int inv_find_item(const InventoryDb *db, const char *code, StockItem *out_item);
int inv_issue_stock(InventoryDb *db, const char *code, float qty, float *out_total_cost);

double inv_calculate_total_investment(const InventoryDb *db);

#endif // INVENTORY_H
