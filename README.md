# Elan-Zephyr Extension: Support for `32f967_dv` Board and UART Driver

This project extends the official Zephyr RTOS to support Elan’s custom board `32f967_dv` and includes a basic UART driver.

---

## 📁 Directory Structure

This project follows the Zephyr module structure. All necessary files are organized to integrate seamlessly with the official Zephyr RTOS.

```
zephyrproject/
├── bootloader/
├── elan-zephyr/ # All files by Elan
├── modules/
├── tools/
└── zephyr/ # Full official Zephyr repo (used for building)
```

```
elan-zephyr/
├── .git/
├── boards/
├── drivers/
├── dts/
├── samples/
├── soc/
├── zephyr/
├── CMakeLists.txt
├── Kconfig
└── README.md
```

---

## 🔧 Prework

1. Clone the full official Zephyr repository into `zephyrproject/zephyr/`  
   → Follow the [Zephyr Getting Started Guide](https://docs.zephyrproject.org/latest/develop/getting_started/index.html)
2. Install Zephyr SDK and required dependencies
3. Clone Elan’s modified files into `zephyrproject/elan-zephyr/`

---

## Build Instructions
After setup, you can build the sample application:

```
cd zephyrproject/elan-zephyr
TEST=west build -b 32f967_dv -p always ./samples/elanspi

The SPI sample will show the sansor id as bellow:
*** Booting Zephyr OS build v4.1.0-6262-g2f2eaf7b6f7f ***
Elan SPI test!
ELAN Read Success 0xb1
```

---

## 📝 Git Commit Guidelines

Please follow these rules when writing commit messages:
```
<type>(scope): <short summary>
```

### ✅ Format Example:

feat(uart): Init board 32f967_dv and UART driver


### ✅ Structure:

- Use **conventional commit types**, such as:
  - `feat`: New features
  - `fix`: Bug fixes
  - `docs`: Documentation updates
  - `refactor`: Code refactoring
  - `test`: Adding or updating tests
  - `chore`: Non-functional changes (e.g., formatting)
- Use parentheses `()` to scope the area of change (e.g., `uart`, `build`, `driver`)
- Use lowercase and concise description after the colon `:`

### ✅ Extended Message (optional but recommended):

After the title line, include details such as:

- Build instructions
- Script usage notes
- Patch or symlink setup steps
- Directory structure

This helps future collaborators (and your future self) understand context quickly.

### ❌ Avoid:

- Vague messages like `update code` or `fix stuff`
- Using full sentences with periods in the title line
- Mixing too many unrelated changes in one commit

---

Example of a good full message:

```
feat(uart): Init board 32f967_dv and UART driver

Build instructions:

cd <path/to/full/zephyr>

west build -b 32f967_dv samples/hello_world/

Set up symbolic links using:
./tools/zephyr_symlinks.sh --mode sync

Apply patches using:
./tools/zephyr_patch.sh --mode apply

Following these practices keeps the repository clean, maintainable, and collaborative.
```
