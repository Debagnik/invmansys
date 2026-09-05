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

## Installation & Prerequisites

To build and run this project, you need a C compiler (`gcc`), `make` (or `mingw32-make`), and a curses library:
- **Windows**: Uses **PDCurses** (`pdcurses_wincon` for console terminal rendering).
- **Linux**: Uses **ncurses**.

---

### Windows Setup (MSYS2 + MinGW-w64 UCRT64)

The recommended environment for building on Windows is **MSYS2** with the **UCRT64** toolchain.

#### Step 1: Install MSYS2
- Download and run the installer from [msys2.org](https://www.msys2.org/), or install via Windows Package Manager:
  ```powershell
  winget install MSYS2.MSYS2
  ```

#### Step 2: Install Required Packages (GCC, Make, PDCurses)
Open the **MSYS2 UCRT64** terminal (or run `C:\msys64\usr\bin\pacman.exe` from PowerShell) and run:
```bash
pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-make mingw-w64-ucrt-x86_64-pdcurses
```
This installs:
- **`gcc`**: Modern C11 compiler (`C:\msys64\ucrt64\bin\gcc.exe`).
- **`mingw32-make`**: GNU Make for Windows (`C:\msys64\ucrt64\bin\mingw32-make.exe`).
- **`pdcurses`**: Public Domain Curses headers and Win32 console/GUI libraries (`libpdcurses_wincon.a`, `libpdcurses.a`).

#### Step 3: Add to Windows PATH
To use `gcc` and `make` from any standard PowerShell or Command Prompt window, add `C:\msys64\ucrt64\bin` to your environment PATH.

In PowerShell (Run as Administrator or for Current User):
```powershell
# For Current User:
[Environment]::SetEnvironmentVariable("Path", [Environment]::GetEnvironmentVariable("Path", "User") + ";C:\msys64\ucrt64\bin", "User")
```
*(After adding, restart your terminal or VS Code to reload the PATH).*

#### Step 4: Configure `make` Command
In MSYS2, GNU Make is installed as `mingw32-make.exe`. To be able to type `make` instead of `mingw32-make`:
```powershell
Copy-Item C:\msys64\ucrt64\bin\mingw32-make.exe C:\msys64\ucrt64\bin\make.exe
```

#### Step 5: Verify Installation
Verify that your toolchain is available in PowerShell:
```powershell
gcc --version
make --version   # or mingw32-make --version
```

---

### Linux Setup

Install the build essentials and `ncurses` development libraries using your distribution's package manager:

**Debian / Ubuntu / Linux Mint:**
```bash
sudo apt update
sudo apt install build-essential libncurses5-dev libncursesw5-dev
```

**Fedora / RHEL / CentOS:**
```bash
sudo dnf install gcc make ncurses-devel
```

**Arch Linux / Manjaro:**
```bash
sudo pacman -S base-devel ncurses
```

---

## Build and Run

### 1. Build the Project
Compile the main application and utility tools using `make` (or `mingw32-make` on Windows):
```bash
make
```
*(On Windows without the alias, you can run `mingw32-make`).*

The compiled binaries will be placed in the `bin/` directory:
- `bin/invmansys` (`invmansys.exe` on Windows): Main inventory management TUI.
- `bin/debagnik2csv` (`debagnik2csv.exe` on Windows): Binary-to-CSV export tool.
- `bin/csv2debagnik` (`csv2debagnik.exe` on Windows): CSV-to-binary import tool.

### 2. Run the Application
Start the TUI application directly:

**Windows (PowerShell / Command Prompt):**
```powershell
.\bin\invmansys.exe
# or via make:
make run
```

**Linux:**
```bash
./bin/invmansys
# or via make:
make run
```

### 3. Clean Build Files
To remove object files and binaries:
```bash
make clean
```
*(The Makefile automatically detects Windows and uses Windows-safe `del` / `rmdir` commands, or `rm` on Linux).*

## Data Import and Export

The project provides two utility tools to convert inventory data between the custom `.debagnik` format and standard CSV files. These tools are built automatically when running `make` and are located in the `bin/` directory.

### Export to CSV (`debagnik2csv`)
To export your existing inventory database to a CSV file:

**Windows:**
```powershell
.\bin\debagnik2csv.exe <input.debagnik> <output.csv>
# Example:
.\bin\debagnik2csv.exe inventory_data.debagnik my_inventory.csv
```

**Linux:**
```bash
./bin/debagnik2csv <input.debagnik> <output.csv>
# Example:
./bin/debagnik2csv inventory_data.debagnik my_inventory.csv
```

### Import from CSV (`csv2debagnik`)
To import data from a CSV file into a new or existing database:

**Windows:**
```powershell
.\bin\csv2debagnik.exe <input.csv> <output.debagnik>
# Example:
.\bin\csv2debagnik.exe my_inventory.csv inventory_data.debagnik
```

**Linux:**
```bash
./bin/csv2debagnik <input.csv> <output.debagnik>
# Example:
./bin/csv2debagnik my_inventory.csv inventory_data.debagnik
```

**Note:** The CSV file should include the following headers for proper mapping: `ItemCode`, `ItemName`, `Unit`, `Price`, `Quantity`, and `ReorderLevel`. The import tool can interactively handle missing headers or gracefully skip records.

## License

This project is licensed under the Apache License 2.0 - see the [LICENSE](LICENSE) file for details.
