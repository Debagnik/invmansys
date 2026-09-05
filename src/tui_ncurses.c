#include "../include/ui.h"
#include "../include/config.h"
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>

static void draw_borders(WINDOW *screen) {
    box(screen, 0, 0);
}

static void show_message_box(const char *title, const char *msg) {
    int h = 7, w = 50;
    int y = (LINES - h) / 2;
    int x = (COLS - w) / 2;
    WINDOW *win = newwin(h, w, y, x);
    box(win, 0, 0);
    mvwprintw(win, 0, (w - strlen(title)) / 2, " %s ", title);
    
    // Simple wrap or center
    mvwprintw(win, 2, 2, "%s", msg);
    mvwprintw(win, h - 2, (w - 20) / 2, "[ Press any key ]");
    
    wrefresh(win);
    wgetch(win);
    delwin(win);
}

static bool get_string_input(WINDOW *win, int y, int x, char *buf, int max_len) {
    keypad(win, TRUE);
    curs_set(1);
    wmove(win, y, x);
    wrefresh(win);
    
    int pos = 0;
    buf[0] = '\0';
    
    while (1) {
        int ch = wgetch(win);
        if (ch == 27) { // ESC key
            curs_set(0);
            return false;
        } else if (ch == '\n' || ch == '\r' || ch == KEY_ENTER) {
            break;
        } else if (ch == KEY_BACKSPACE || ch == 127 || ch == '\b') {
            if (pos > 0) {
                pos--;
                buf[pos] = '\0';
                mvwaddch(win, y, x + pos, ' ');
                wmove(win, y, x + pos);
                wrefresh(win);
            }
        } else if (ch >= 32 && ch <= 126 && pos < max_len - 1) {
            buf[pos++] = (char)ch;
            buf[pos] = '\0';
            mvwaddch(win, y, x + pos - 1, ch);
            wrefresh(win);
        }
    }
    
    curs_set(0);
    return true;
}

static bool get_float_input(WINDOW *win, int y, int x, float *val) {
    char buf[32];
    if (!get_string_input(win, y, x, buf, sizeof(buf))) return false;
    if (strlen(buf) > 0) *val = strtof(buf, NULL);
    return true;
}

static bool get_int_input(WINDOW *win, int y, int x, int *val) {
    char buf[32];
    if (!get_string_input(win, y, x, buf, sizeof(buf))) return false;
    if (strlen(buf) > 0) *val = (int)strtol(buf, NULL, 10);
    return true;
}

static void handle_add_item(InventoryDb *db) {
    int h = 15, w = 60;
    int y = (LINES - h) / 2;
    int x = (COLS - w) / 2;
    WINDOW *win = newwin(h, w, y, x);
    box(win, 0, 0);
    mvwprintw(win, 0, 2, " Add Item ");

    StockItem item;
    memset(&item, 0, sizeof(StockItem));

    mvwprintw(win, 2, 2, "Item Code    : ");
    mvwprintw(win, 4, 2, "Item Name    : ");
    mvwprintw(win, 6, 2, "Unit (String): ");
    mvwprintw(win, 8, 2, "Rate/Price   : ");
    mvwprintw(win, 10,2, "Quantity     : ");
    mvwprintw(win, 12,2, "Reorder Level: ");
    wrefresh(win);

    if (!get_string_input(win, 2, 17, item.itemcode, MAX_CODE_LEN)) { delwin(win); return; }
    if (!get_string_input(win, 4, 17, item.itemname, MAX_NAME_LEN)) { delwin(win); return; }
    if (!get_string_input(win, 6, 17, item.unit, MAX_UNIT_LEN)) { delwin(win); return; }
    if (!get_float_input(win, 8, 17, &item.itemrate)) { delwin(win); return; }
    if (!get_float_input(win, 10,17, &item.itemqty)) { delwin(win); return; }
    if (!get_int_input(win, 12,17, &item.minqty)) { delwin(win); return; }

    int res = inv_add_item(db, &item);
    delwin(win);

    if (res == 0) {
        show_message_box("Success", "Item added successfully.");
    } else if (res == -2) {
        show_message_box("Error", "Item code already exists.");
    } else {
        show_message_box("Error", "Failed to add item.");
    }
}

static void handle_edit_item(InventoryDb *db) {
    int h = 15, w = 60;
    int y = (LINES - h) / 2;
    int x = (COLS - w) / 2;
    WINDOW *win = newwin(h, w, y, x);
    box(win, 0, 0);
    mvwprintw(win, 0, 2, " Edit Item ");
    
    char code[MAX_CODE_LEN];
    mvwprintw(win, 2, 2, "Enter Item Code to Edit: ");
    wrefresh(win);
    if (!get_string_input(win, 2, 27, code, MAX_CODE_LEN)) { delwin(win); return; }

    StockItem item;
    if (inv_find_item(db, code, &item) < 0) {
        delwin(win);
        show_message_box("Error", "Item not found.");
        return;
    }

    wclear(win);
    box(win, 0, 0);
    mvwprintw(win, 0, 2, " Edit Item: %s ", item.itemcode);
    mvwprintw(win, 2, 2, "Name (curr: %s): ", item.itemname);
    mvwprintw(win, 4, 2, "Unit (curr: %s): ", item.unit);
    mvwprintw(win, 6, 2, "Rate (curr: %.2f): ", item.itemrate);
    mvwprintw(win, 8, 2, "Qty  (curr: %.2f): ", item.itemqty);
    mvwprintw(win, 10,2, "Min  (curr: %d): ", item.minqty);
    wrefresh(win);

    char buf[64];
    if (!get_string_input(win, 2, 20 + strlen(item.itemname), buf, MAX_NAME_LEN)) { delwin(win); return; }
    if (strlen(buf) > 0) strcpy(item.itemname, buf);

    if (!get_string_input(win, 4, 20 + strlen(item.unit), buf, MAX_UNIT_LEN)) { delwin(win); return; }
    if (strlen(buf) > 0) strcpy(item.unit, buf);

    if (!get_string_input(win, 6, 20 + 5, buf, 32)) { delwin(win); return; }
    if (strlen(buf) > 0) item.itemrate = strtof(buf, NULL);

    if (!get_string_input(win, 8, 20 + 5, buf, 32)) { delwin(win); return; }
    if (strlen(buf) > 0) item.itemqty = strtof(buf, NULL);

    if (!get_string_input(win, 10,20 + 3, buf, 32)) { delwin(win); return; }
    if (strlen(buf) > 0) item.minqty = (int)strtol(buf, NULL, 10);

    inv_update_item(db, &item);
    delwin(win);
    show_message_box("Success", "Item updated.");
}

static void handle_show_item(InventoryDb *db) {
    int h = 10, w = 50;
    int y = (LINES - h) / 2;
    int x = (COLS - w) / 2;
    WINDOW *win = newwin(h, w, y, x);
    box(win, 0, 0);
    mvwprintw(win, 0, 2, " Search Item ");
    
    char code[MAX_CODE_LEN];
    mvwprintw(win, 2, 2, "Enter Item Code: ");
    wrefresh(win);
    if (!get_string_input(win, 2, 19, code, MAX_CODE_LEN)) { delwin(win); return; }

    StockItem item;
    if (inv_find_item(db, code, &item) < 0) {
        delwin(win);
        show_message_box("Error", "Item not found.");
        return;
    }

    wclear(win);
    box(win, 0, 0);
    mvwprintw(win, 0, 2, " Item Details ");
    mvwprintw(win, 2, 2, "Code : %s", item.itemcode);
    mvwprintw(win, 3, 2, "Name : %s", item.itemname);
    mvwprintw(win, 4, 2, "Unit : %s", item.unit);
    mvwprintw(win, 5, 2, "Rate : %.2f", item.itemrate);
    mvwprintw(win, 6, 2, "Stock: %.2f", item.itemqty);
    mvwprintw(win, 7, 2, "Reord: %d", item.minqty);
    wrefresh(win);
    wgetch(win);
    delwin(win);
}

static void handle_stock_report(InventoryDb *db) {
    int h = LINES - 4;
    int w = COLS - 4;
    int y = 2;
    int x = 2;
    WINDOW *win = newwin(h, w, y, x);
    keypad(win, TRUE);
    
    int scroll_offset = 0;
    
    while (1) {
        wclear(win);
        box(win, 0, 0);
        mvwprintw(win, 0, 2, " Stock Report ");

        mvwprintw(win, 2, 2, "%-12s | %-20s | %-10s | %-8s | %-8s | %-8s | %-10s", 
                  "Code", "Name", "Unit", "Rate", "Stock", "Reord", "Total");
        mvwhline(win, 3, 1, ACS_HLINE, w - 2);

        int max_rows = h - 7;
        if (scroll_offset < 0) scroll_offset = 0;
        if (db->count > 0 && scroll_offset > (int)db->count - max_rows) {
            scroll_offset = (int)db->count - max_rows;
            if (scroll_offset < 0) scroll_offset = 0;
        }

        int row = 4;
        for (size_t i = scroll_offset; i < db->count && row < h - 3; i++) {
            StockItem *item = &db->items[i];
            double total = (double)item->itemrate * (double)item->itemqty;
            mvwprintw(win, row++, 2, "%-12s | %-20.20s | %-10.10s | %-8.2f | %-8.2f | %-8d | %-10.2f",
                      item->itemcode, item->itemname, item->unit, item->itemrate, item->itemqty, item->minqty, total);
        }
        
        mvwhline(win, h - 3, 1, ACS_HLINE, w - 2);
        mvwprintw(win, h - 2, 2, "Total Items: %zu   Total Investment: %.2f", db->count, inv_calculate_total_investment(db));
        mvwprintw(win, h - 2, w - 25, "UP/DOWN scroll, Q quit");

        wrefresh(win);
        int ch = wgetch(win);
        if (ch == KEY_DOWN) {
            scroll_offset++;
        } else if (ch == KEY_UP) {
            scroll_offset--;
        } else if (ch == 'q' || ch == 'Q' || ch == 27 || ch == '\n' || ch == '\r') {
            break;
        }
    }
    delwin(win);
}

static void handle_issue_item(InventoryDb *db) {
    int h = 12, w = 50;
    int y = (LINES - h) / 2;
    int x = (COLS - w) / 2;
    WINDOW *win = newwin(h, w, y, x);
    box(win, 0, 0);
    mvwprintw(win, 0, 2, " Issue Item ");
    
    char code[MAX_CODE_LEN];
    mvwprintw(win, 2, 2, "Enter Item Code: ");
    wrefresh(win);
    if (!get_string_input(win, 2, 19, code, MAX_CODE_LEN)) { delwin(win); return; }

    StockItem item;
    if (inv_find_item(db, code, &item) < 0) {
        delwin(win);
        show_message_box("Error", "Item not found.");
        return;
    }

    mvwprintw(win, 4, 2, "Available: %.2f %s", item.itemqty, item.unit);
    mvwprintw(win, 5, 2, "Quantity to issue: ");
    wrefresh(win);

    float qty = 0.0f;
    if (!get_float_input(win, 5, 21, &qty)) { delwin(win); return; }

    float cost = 0;
    int res = inv_issue_stock(db, code, qty, &cost);
    delwin(win);

    if (res == 0) {
        char msg[128];
        snprintf(msg, sizeof(msg), "Issued %.2f %s\nTotal Bill: %.2f", qty, item.unit, cost);
        show_message_box("Success", msg);
    } else if (res == -3) {
        show_message_box("Error", "Insufficient overall stock!");
    } else if (res == -4) {
        show_message_box("Warning", "Cannot issue: Stock would drop below reorder level!");
    } else {
        show_message_box("Error", "Failed to issue item.");
    }
}

static void handle_reorder_alerts(InventoryDb *db) {
    int h = LINES - 4;
    int w = COLS - 4;
    int y = 2;
    int x = 2;
    WINDOW *win = newwin(h, w, y, x);
    keypad(win, TRUE);

    int scroll_offset = 0;
    size_t *alert_indices = malloc(sizeof(size_t) * (db->count + 1));
    int alert_count = 0;
    for (size_t i = 0; i < db->count; i++) {
        if (db->items[i].itemqty <= db->items[i].minqty) {
            alert_indices[alert_count++] = i;
        }
    }

    while (1) {
        wclear(win);
        box(win, 0, 0);
        mvwprintw(win, 0, 2, " Reorder Alerts ");

        mvwprintw(win, 2, 2, "%-12s | %-20s | %-8s | %-8s", "Code", "Name", "Stock", "Reord");
        mvwhline(win, 3, 1, ACS_HLINE, w - 2);

        int row = 4;
        
        if (alert_count == 0) {
            mvwprintw(win, row + 2, (w - 20) / 2, "No items to reorder!");
        } else {
            int max_rows = h - 6;
            if (scroll_offset < 0) scroll_offset = 0;
            if (alert_count > 0 && scroll_offset > alert_count - max_rows) {
                scroll_offset = alert_count - max_rows;
                if (scroll_offset < 0) scroll_offset = 0;
            }

            for (int i = scroll_offset; i < alert_count && row < h - 2; i++) {
                StockItem *item = &db->items[alert_indices[i]];
                mvwprintw(win, row++, 2, "%-12s | %-20.20s | %-8.2f | %-8d",
                          item->itemcode, item->itemname, item->itemqty, item->minqty);
            }
            mvwprintw(win, h - 2, w - 25, "UP/DOWN scroll, Q quit");
        }

        wrefresh(win);
        int ch = wgetch(win);
        if (ch == KEY_DOWN) {
            scroll_offset++;
        } else if (ch == KEY_UP) {
            scroll_offset--;
        } else if (ch == 'q' || ch == 'Q' || ch == 27 || ch == '\n' || ch == '\r') {
            break;
        }
    }
    
    free(alert_indices);
    delwin(win);
}

void ui_init(void) {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    mousemask(ALL_MOUSE_EVENTS, NULL);
    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_WHITE, COLOR_BLUE);
        init_pair(2, COLOR_YELLOW, COLOR_BLUE);
        init_pair(3, COLOR_BLUE, COLOR_CYAN);
        bkgd(COLOR_PAIR(1));
    }
}

void ui_shutdown(void) {
    endwin();
}

void ui_run(InventoryDb *db) {
    int ch;
    int selected_option = 1;
    const int num_options = 7;
    const char *menu_options[] = {
        "1: Add an item",
        "2: Edit item Information",
        "3: Show Item Information",
        "4: View Stock Report",
        "5: Issue Items from Stock",
        "6: View Items to be ordered",
        "0: Close the application"
    };
    int option_keys[] = {1, 2, 3, 4, 5, 6, 0};

    while (1) {
        clear();
        draw_borders(stdscr);
        
        attron(A_BOLD | COLOR_PAIR(2));
        mvprintw(1, (COLS - strlen(APP_NAME)) / 2, "%s", APP_NAME);
        attroff(A_BOLD | COLOR_PAIR(2));
        
        for (int i = 0; i < num_options; i++) {
            int y_pos = 4 + i;
            if (i == 6) y_pos = 11;
            
            if (selected_option == option_keys[i]) {
                attron(COLOR_PAIR(3) | A_BOLD);
                mvprintw(y_pos, 5, "%s", menu_options[i]);
                attroff(COLOR_PAIR(3) | A_BOLD);
            } else {
                mvprintw(y_pos, 5, "%s", menu_options[i]);
            }
        }

        mvprintw(15, 5, "Total Items: %zu", db->count);

        refresh();
        ch = getch();

        if (ch == KEY_DOWN) {
            int idx = -1;
            for (int i = 0; i < num_options; i++) if (option_keys[i] == selected_option) idx = i;
            idx = (idx + 1) % num_options;
            selected_option = option_keys[idx];
        } else if (ch == KEY_UP) {
            int idx = -1;
            for (int i = 0; i < num_options; i++) if (option_keys[i] == selected_option) idx = i;
            idx = (idx - 1 + num_options) % num_options;
            selected_option = option_keys[idx];
        } else if (ch == '\n' || ch == '\r' || ch == KEY_ENTER) {
            ch = selected_option + '0';
        } else if (ch == KEY_MOUSE) {
            MEVENT event;
            if (getmouse(&event) == OK) {
                FILE *log_file = fopen("error.log", "a");
                if (log_file) {
                    fprintf(log_file, "DEBUG: Mouse event bstate=0x%lx, x=%d, y=%d\n", (unsigned long)event.bstate, event.x, event.y);
                    fclose(log_file);
                }
                if (event.bstate & BUTTON1_CLICKED || event.bstate & BUTTON1_PRESSED) {
                    for (int i = 0; i < num_options; i++) {
                        int y_pos = 4 + i;
                        if (i == 6) y_pos = 11;
                        if (event.y == y_pos && event.x >= 5 && event.x < 5 + (int)strlen(menu_options[i])) {
                            selected_option = option_keys[i];
                            ch = selected_option + '0'; // auto trigger
                            break;
                        }
                    }
                }
            }
        } else if (ch >= '0' && ch <= '6') {
            selected_option = ch - '0';
        }

        if (ch == '0') {
            break;
        } else if (ch == '1') {
            handle_add_item(db);
        } else if (ch == '2') {
            handle_edit_item(db);
        } else if (ch == '3') {
            handle_show_item(db);
        } else if (ch == '4') {
            handle_stock_report(db);
        } else if (ch == '5') {
            handle_issue_item(db);
        } else if (ch == '6') {
            handle_reorder_alerts(db);
        }
    }
}
