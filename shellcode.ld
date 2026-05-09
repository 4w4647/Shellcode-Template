ENTRY(shellcode)
SECTIONS
{
	. = 0x140001000;

	.text :
	{
		*(.text.shellcode)
		*(.text*)
	}

	/DISCARD/ : { *(*) }
}