#include "../include/storage.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_LINE 2048
#define MAX_COLS 32

void trim_newline(char *str) {
    size_t len = strlen(str);
    while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r')) {
        str[len - 1] = '\0';
        len--;
    }
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input.csv> <output.debagnik>\n", argv[0]);
        return 1;
    }

    const char *in_file = argv[1];
    const char *out_file = argv[2];

    FILE *f = fopen(in_file, "r");
    if (!f) {
        fprintf(stderr, "Error: Could not open CSV file %s\n", in_file);
        return 1;
    }

    char header_line[MAX_LINE];
    if (!fgets(header_line, sizeof(header_line), f)) {
        fprintf(stderr, "Error: CSV file is empty\n");
        fclose(f);
        return 1;
    }
    trim_newline(header_line);

    char *headers[MAX_COLS];
    int num_headers = 0;
    
    char header_copy[MAX_LINE];
    strcpy(header_copy, header_line);
    
    char *token = strtok(header_copy, ",");
    while (token && num_headers < MAX_COLS) {
        headers[num_headers++] = token;
        token = strtok(NULL, ",");
    }

    int col_code = -1, col_name = -1, col_unit = -1, col_price = -1, col_qty = -1, col_minqty = -1;
    for (int i = 0; i < num_headers; i++) {
        if (strcasecmp(headers[i], "ItemCode") == 0) col_code = i;
        else if (strcasecmp(headers[i], "ItemName") == 0) col_name = i;
        else if (strcasecmp(headers[i], "Unit") == 0) col_unit = i;
        else if (strcasecmp(headers[i], "Price") == 0) col_price = i;
        else if (strcasecmp(headers[i], "Quantity") == 0) col_qty = i;
        else if (strcasecmp(headers[i], "ReorderLevel") == 0) col_minqty = i;
    }

    int missing_count = 0;
    const char *missing_names[6];
    if (col_code == -1) missing_names[missing_count++] = "ItemCode";
    if (col_name == -1) missing_names[missing_count++] = "ItemName";
    if (col_unit == -1) missing_names[missing_count++] = "Unit";
    if (col_price == -1) missing_names[missing_count++] = "Price";
    if (col_qty == -1) missing_names[missing_count++] = "Quantity";
    if (col_minqty == -1) missing_names[missing_count++] = "ReorderLevel";

    if (missing_count > 0) {
        printf("Warning: The following required headers are missing in the CSV:\n");
        for (int i = 0; i < missing_count; i++) {
            printf(" - %s\n", missing_names[i]);
        }
        printf("\nDo you want to proceed anyway with empty/default values? (y/n): ");
        char ans[10];
        if (!fgets(ans, sizeof(ans), stdin)) ans[0] = 'n';
        if (tolower(ans[0]) != 'y') {
            printf("Do you want to automatically add the missing headers to the CSV file? (y/n): ");
            if (fgets(ans, sizeof(ans), stdin) && tolower(ans[0]) == 'y') {
                fclose(f);
                // Rewrite CSV
                FILE *fin = fopen(in_file, "r");
                FILE *ftmp = fopen("temp_csv_import.csv", "w");
                if (fin && ftmp) {
                    char line[MAX_LINE];
                    if (fgets(line, sizeof(line), fin)) {
                        trim_newline(line);
                        fprintf(ftmp, "%s", line);
                        for (int i = 0; i < missing_count; i++) {
                            fprintf(ftmp, ",%s", missing_names[i]);
                        }
                        fprintf(ftmp, "\n");
                        
                        while (fgets(line, sizeof(line), fin)) {
                            trim_newline(line);
                            fprintf(ftmp, "%s", line);
                            for (int i = 0; i < missing_count; i++) {
                                fprintf(ftmp, ","); // empty columns
                            }
                            fprintf(ftmp, "\n");
                        }
                    }
                    fclose(fin);
                    fclose(ftmp);
                    remove(in_file);
                    rename("temp_csv_import.csv", in_file);
                    printf("Missing headers have been added to %s.\n", in_file);
                } else {
                    printf("Failed to rewrite CSV.\n");
                    if (fin) fclose(fin);
                    if (ftmp) fclose(ftmp);
                }
            } else {
                fclose(f);
            }
            printf("Exiting import.\n");
            return 0;
        }
    }

    InventoryDb *db = inv_db_create();
    if (!db) {
        fprintf(stderr, "Memory error.\n");
        fclose(f);
        return 1;
    }
    
    // We attempt to load the existing DB first so we can append/update, 
    // or just start fresh if it doesn't exist
    storage_load_db(out_file, db);

    int rows_imported = 0;
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), f)) {
        trim_newline(line);
        if (strlen(line) == 0) continue;

        char *cols[MAX_COLS];
        int num_cols = 0;
        char *start = line;
        while (*start && num_cols < MAX_COLS) {
            char *comma = strchr(start, ',');
            if (comma) {
                *comma = '\0';
                cols[num_cols++] = start;
                start = comma + 1;
            } else {
                cols[num_cols++] = start;
                break;
            }
        }
        
        // Ensure we have enough empty strings for missing trailing columns
        while (num_cols < num_headers) {
            cols[num_cols++] = "";
        }

        StockItem item;
        memset(&item, 0, sizeof(StockItem));
        
        if (col_code != -1 && col_code < num_cols) {
            strncpy(item.itemcode, cols[col_code], MAX_CODE_LEN - 1);
        }
        if (col_name != -1 && col_name < num_cols) {
            strncpy(item.itemname, cols[col_name], MAX_NAME_LEN - 1);
        }
        if (col_unit != -1 && col_unit < num_cols) {
            strncpy(item.unit, cols[col_unit], MAX_UNIT_LEN - 1);
        }
        if (col_price != -1 && col_price < num_cols && strlen(cols[col_price]) > 0) {
            item.itemrate = strtof(cols[col_price], NULL);
        }
        if (col_qty != -1 && col_qty < num_cols && strlen(cols[col_qty]) > 0) {
            item.itemqty = strtof(cols[col_qty], NULL);
        }
        if (col_minqty != -1 && col_minqty < num_cols && strlen(cols[col_minqty]) > 0) {
            item.minqty = (int)strtol(cols[col_minqty], NULL, 10);
        }

        // Add or update
        if (inv_add_item(db, &item) == -2) { // Duplicate
            inv_update_item(db, &item);
        }
        rows_imported++;
    }

    fclose(f);

    if (storage_save_db(out_file, db) == 0) {
        printf("Successfully imported %d records from %s into %s\n", rows_imported, in_file, out_file);
    } else {
        printf("Failed to save imported database %s\n", out_file);
    }

    inv_db_free(db);
    return 0;
}
