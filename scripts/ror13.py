import argparse

MASK = 0xFFFFFFFF

def ror13(v: int) -> int:
    return ((v >> 13) | (v << 19)) & MASK

def ror13_a(name: str) -> int:
    h = 0
    for c in name:
        h = ror13(h)
        h = (h + ord(c.lower())) & MASK
    return h

def ror13_w(name: str) -> int:
    h = 0
    for c in name:
        h = ror13(h)
        h = (h + ord(c.lower())) & MASK
    return h

def main():
    ap = argparse.ArgumentParser(
        description='ROR13 hasher for shellcode import resolution'
    )
    ap.add_argument('-m', '--module', metavar='DLL',
                    help='Module name (e.g. kernel32.dll)')
    ap.add_argument('-s', '--symbol', metavar='FUNC',
                    help='Exported symbol name (e.g. WinExec)')
    args = ap.parse_args()

    if not args.module and not args.symbol:
        ap.error('provide --module and/or --symbol')

    if args.module:
        print(f'[module] {args.module!r} -> 0x{ror13_w(args.module):08X}')
    if args.symbol:
        print(f'[symbol] {args.symbol!r} -> 0x{ror13_a(args.symbol):08X}')

if __name__ == '__main__':
    main()