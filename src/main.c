#include "../include/config.h"
#include "../include/inventory.h"
#include "../include/storage.h"
#include "../include/ui.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    const char *db_path = DB_FILE;
    if (argc > 1) {
        db_path = argv[1];
    }

    InventoryDb *db = inv_db_create();
    if (!db) {
        fprintf(stderr, "Failed to initialize inventory db.\n");
        return 1;
    }

    if (storage_load_db(db_path, db) < 0) {
        fprintf(stderr, "Failed to load database: %s\n", db_path);
        inv_db_free(db);
        return 1;
    }

    ui_init();
    
    // Pass control to UI
    ui_run(db);

    ui_shutdown();

    // Save and backup on exit
    storage_save_db(db_path, db);
    storage_backup_db(db_path, DB_BACKUP_FILE);

    inv_db_free(db);
    return 0;
}
