#ifndef UI_H
#define UI_H

#include "inventory.h"
#include <stdbool.h>

void ui_init(void);
void ui_shutdown(void);

// Main entry point for UI loop
void ui_run(InventoryDb *db);

#endif // UI_H
