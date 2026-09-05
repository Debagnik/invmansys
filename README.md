# invmansys
(from c_rep)

A Terminal-based Inventory Management System written in C.

## Description

`invmansys` is a fast and lightweight inventory management tool built using the `ncurses` library for its Terminal User Interface (TUI). It allows users to manage inventory records, read/write custom storage formats (e.g., `.debagnik` files), and includes utility tools to convert data to and from CSV.

## Project Structure

```
invmansys/
├── bin/          # Compiled binaries (generated after build)
├── include/      # C header files (.h)
├── src/          # C source files (.c) including core logic and ncurses TUI
├── tools/        # Utility source files (e.g., debagnik2csv, csv2debagnik)
├── legacy/       # Legacy files/code
├── Makefile      # Build configuration
├── LICENSE       # Apache 2.0 License
└── README.md     # Project documentation
```

## Installation

### Prerequisites

To build and run this project, you will need a C compiler (`gcc`), `make` (or `mingw32-make` on Windows), and the curses development library:
- On **Linux**, `ncurses` is used.
- On **Windows**, `PDCurses` is used.

**Windows (MSYS2 / MinGW-w64 UCRT64):**
```bash
pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-make mingw-w64-ucrt-x86_64-pdcurses
```

**Debian/Ubuntu-based:**
```bash
sudo apt update
sudo apt install build-essential libncurses5-dev libncursesw5-dev
```

**Fedora/RHEL-based:**
```bash
sudo dnf install gcc make ncurses-devel
```

**Arch Linux:**
```bash
sudo pacman -S base-devel ncurses
```

### Build and Run

1. **Clone the repository:**
   *(Assuming you have already downloaded the repository)*
   ```bash
   cd invmansys
   ```

2. **Build the project:**
   Compile the main application and utility tools using `make` (or `mingw32-make` on Windows):
   ```bash
   make
   # On Windows:
   # mingw32-make
   ```
   The compiled binaries will be placed in the `bin/` directory.

3. **Run the application:**
   You can start the inventory management system using:
   ```bash
   make run
   # or run directly: ./bin/invmansys (Linux) or .\bin\invmansys.exe (Windows)
   ```

4. **Clean build files (optional):**
   ```bash
   make clean
   ```

## Data Import and Export

The project provides two utility tools to convert inventory data between the custom `.debagnik` format and standard CSV files. These tools are built automatically when running `make` and are located in the `bin/` directory.

### Export to CSV (`debagnik2csv`)
To export your existing inventory database to a CSV file:
```bash
./bin/debagnik2csv <input.debagnik> <output.csv>
```
*Example:*
```bash
./bin/debagnik2csv inventory_data.debagnik my_inventory.csv
```

### Import from CSV (`csv2debagnik`)
To import data from a CSV file into a new or existing database:
```bash
./bin/csv2debagnik <input.csv> <output.debagnik>
```
*Example:*
```bash
./bin/csv2debagnik my_inventory.csv inventory_data.debagnik
```
**Note:** The CSV file should include the following headers for proper mapping: `ItemCode`, `ItemName`, `Unit`, `Price`, `Quantity`, and `ReorderLevel`. The import tool can interactively handle missing headers or gracefully skip records.

## License

This project is licensed under the Apache License 2.0 - see the [LICENSE](LICENSE) file for details.
