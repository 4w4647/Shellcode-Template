#include "peb.h"

DWORD ror13(DWORD v) {
    return (v >> 13) | (v << (32 - 13));
}

DWORD ror13_a(LPCSTR s) {
    DWORD h = 0;

    while (*s) {
        CHAR c = *s++;

        if (c >= 'A' && c <= 'Z')
            c += 0x20;

        h = ror13(h);
        h += (BYTE)c;
    }

    return h;
}

DWORD ror13_w(LPCWSTR s) {
    DWORD h = 0;

    while (*s) {
        WCHAR c = *s++;

        if (c >= L'A' && c <= L'Z')
            c += 0x20;

        h = ror13(h);
        h += (WORD)c;
    }

    return h;
}

HMODULE peb_get_module(DWORD hash) {
    PPEB peb = (PPEB)__readgsqword(0x60);

    if (!peb || !peb->Ldr)
        return NULL;

    PLIST_ENTRY head = &peb->Ldr->InMemoryOrderModuleList;

    for (PLIST_ENTRY e = head->Flink; e != head; e = e->Flink) {
        PLDR_DATA_TABLE_ENTRY entry =
            CONTAINING_RECORD(e, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);

        if (!entry->BaseDllName.Buffer)
            continue;

        if (ror13_w(entry->BaseDllName.Buffer) == hash)
            return entry->DllBase;
    }

    return NULL;
}

FARPROC peb_get_symbol(HMODULE module, DWORD hash) {
    if (!module)
        return NULL;

    PBYTE base = (PBYTE)module;

    PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)base;
    if (dos->e_magic != IMAGE_DOS_SIGNATURE)
        return NULL;

    PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(base + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE)
        return NULL;

    IMAGE_DATA_DIRECTORY expDir =
        nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];

    if (!expDir.VirtualAddress)
        return NULL;

    PIMAGE_EXPORT_DIRECTORY exp =
        (PIMAGE_EXPORT_DIRECTORY)(base + expDir.VirtualAddress);

    PDWORD names     = (PDWORD)(base + exp->AddressOfNames);
    PWORD  ordinals  = (PWORD)(base + exp->AddressOfNameOrdinals);
    PDWORD functions = (PDWORD)(base + exp->AddressOfFunctions);

    for (DWORD i = 0; i < exp->NumberOfNames; i++) {
        LPCSTR func = (LPCSTR)(base + names[i]);

        if (ror13_a(func) == hash) {
            WORD ord = ordinals[i];
            if (ord >= exp->NumberOfFunctions)
                return NULL;
            DWORD rva = functions[ord];

            if (rva >= expDir.VirtualAddress &&
                rva <  expDir.VirtualAddress + expDir.Size)
                return NULL;

            return (FARPROC)(base + rva);
        }
    }

    return NULL;
}