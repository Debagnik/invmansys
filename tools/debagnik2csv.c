#include "../include/storage.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input.debagnik> <output.csv>\n", argv[0]);
        return 1;
    }

    const char *in_file = argv[1];
    const char *out_file = argv[2];

    InventoryDb *db = inv_db_create();
    if (!db) {
        fprintf(stderr, "Failed to initialize memory\n");
        return 1;
    }

    if (storage_load_db(in_file, db) < 0) {
        fprintf(stderr, "Failed to load database %s\n", in_file);
        inv_db_free(db);
        return 1;
    }

    FILE *out = fopen(out_file, "w");
    if (!out) {
        fprintf(stderr, "Failed to open output file %s\n", out_file);
        inv_db_free(db);
        return 1;
    }

    fprintf(out, "ItemCode,ItemName,Unit,Price,Quantity,ReorderLevel,TotalValue\n");
    for (size_t i = 0; i < db->count; i++) {
        StockItem *item = &db->items[i];
        double total_val = (double)item->itemqty * (double)item->itemrate;
        fprintf(out, "%s,%s,%s,%.2f,%.2f,%d,%.2f\n",
                item->itemcode, item->itemname, item->unit,
                item->itemrate, item->itemqty, item->minqty, total_val);
    }

    fclose(out);
    inv_db_free(db);
    printf("Successfully exported %s to %s\n", in_file, out_file);
    return 0;
}
