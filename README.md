# Shellcode-Template

Position-independent Windows x64 shellcode template using PEB walking, ROR13 API hashing, manual export parsing, and stack strings for fully dynamic API resolution without imports.

---

## Features

- Windows x64 PIC shellcode
- No imports / no IAT
- No CRT dependencies
- PEB module enumeration
- Manual PE export parsing
- ROR13 API hashing
- Stack strings stored in `.text`
- Single-section shellcode extraction
- Minimal build pipeline
- Raw shellcode binary output

---

## Overview

The template resolves Windows APIs dynamically at runtime by:

1. Reading the PEB from `GS:[0x60]`
2. Enumerating loaded modules through `PEB_LDR_DATA`
3. Locating modules via ROR13 hashes
4. Parsing PE export tables manually
5. Resolving exports through hashed names

This avoids:
- static imports
- relocation dependencies
- embedded import strings

---

## Example

```c
HMODULE kernel32 = peb_get_module(kernel32_hash);

WinExec_t *WinExec = (WinExec_t *)
    peb_get_symbol(kernel32, win_exec_hash);

char calc_exe[] = {
    'c','a','l','c',
    '.','e','x','e',
    0
};

WinExec(calc_exe, SW_SHOW);
```

---

## Repository Layout

```text
.
├── include/
│   └── peb.h
├── src/
│   ├── main.c
│   └── peb.c
├── scripts/
│   └── ror13.py
├── Makefile
└── shellcode.ld
```

---

# Building

## Requirements

- x86_64-w64-mingw32-gcc
- x86_64-w64-mingw32-ld
- x86_64-w64-mingw32-objcopy
- python3
- make

### Debian

```bash
sudo apt install mingw-w64 python3 make -y
```

---

## Build

```bash
make
```

Outputs:

```text
build/shellcode.exe
build/shellcode.bin
```

---

# Hash Generation

The project includes a ROR13 hashing utility for module and export names.

## Usage

```bash
python3 scripts/ror13.py -m kernel32.dll
python3 scripts/ror13.py -s WinExec
```

Example:

```text
[module] 'kernel32.dll' -> 0x8FECD63F
[symbol] 'WinExec' -> 0x4F0AFE98
```

---

# API Resolution

## Module Resolution

Modules are resolved by traversing:

```text
PEB
 └── Ldr
      └── InMemoryOrderModuleList
```

Each module name is hashed using:

```c
DWORD ror13_w(LPCWSTR s);
```

---

## Export Resolution

Exports are resolved manually through:

```text
IMAGE_DOS_HEADER
IMAGE_NT_HEADERS
IMAGE_EXPORT_DIRECTORY
```

Export names are hashed using:

```c
DWORD ror13_a(LPCSTR s);
```

No calls to:
- `GetProcAddress`
- `LoadLibraryA`

are required during bootstrap.

---

# Stack Strings

Strings are constructed directly on the stack:

```c
char calc_exe[] = {
    'c','a','l','c',
    '.','e','x','e',
    0
};
```

This prevents string placement inside:
- `.rdata`
- `.data`

and keeps shellcode self-contained inside `.text`.

---

# Linker Script

The linker script ensures:
- a single executable section
- stripped output
- discarded metadata sections

```ld
.text :
{
    *(.text.shellcode)
    *(.text*)
}
```

All other sections are discarded.

---

# Extracting Shellcode

The final shellcode is extracted directly from `.text`:

```bash
x86_64-w64-mingw32-objcopy -O binary -j .text shellcode.exe shellcode.bin
```

The resulting file:
- is raw position-independent shellcode
- contains no PE headers
- can be injected or executed directly

---

# Compiler Flags

Key build flags:

```bash
-nostdlib
-fno-stack-protector
-fno-asynchronous-unwind-tables
-ffunction-sections
```

These remove:
- CRT initialization
- unwind metadata
- stack cookies
- unnecessary runtime artifacts

---

# Notes

- Designed for educational and low-level Windows internals research
- Tested on Windows x64
- Assumes standard Windows loader structures
- Forwarded exports are currently ignored
- API hashing is case-insensitive