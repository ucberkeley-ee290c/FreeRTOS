ENTRY(_start)

MEMORY {
  SRAM(rwx): ORIGIN = 0x80000000, LENGTH = 24K
}

SECTIONS
{
	.init :
	{
		_text = .;
		KEEP (*(SORT_NONE(.init)))
	} >SRAM
	
	.text :
	{
		*(.text.unlikely .text.unlikely.*)
		*(.text.startup .text.startup.*)
		*(.text .text.*)
		*(.gnu.linkonce.t.*)
	} >SRAM
	
	.fini :
	{
		KEEP (*(SORT_NONE(.fini)))
		_etext = .;
	} >SRAM

	.rodata.align :
	{
		. = ALIGN(4);
		_rodata = .;
	} >SRAM

	.rodata.start :
	{
		_rodata_lma = LOADADDR(.rodata.start);
	} >SRAM

	.rodata :
	{
		*(.rdata)
		*(.rodata .rodata.*)
		*(.gnu.linkonce.r.*)

		. = ALIGN(4);
		_erodata = .;
	} >SRAM

	.data.align :
	{
		. = ALIGN(4);
		_data = .;
	} >SRAM

	.data.start :
	{
		_data_lma = LOADADDR(.data.start);
	} >SRAM

	.data :
	{
		*(.data .data.*)
		*(.gnu.linkonce.d.*)
		. = ALIGN(8);
		PROVIDE( __global_pointer$ = . + 0x800 );
		*(.sdata .sdata.*)
		*(.sdata2 .sdata2.*)
		*(.gnu.linkonce.s.*)
		. = ALIGN(8);
		*(.srodata.cst16)
		*(.srodata.cst8)
		*(.srodata.cst4)
		*(.srodata.cst2)
		*(.srodata .srodata.*)

		. = ALIGN(4);
		_edata = .;
	} >SRAM

	.bss.align :
	{
		. = ALIGN(4);
		_bss = .;
	} >SRAM

	.bss.start :
	{
		_bss_lma = LOADADDR(.bss.start);
	} >SRAM

	.bss :
	{
		*(.sbss*)
		*(.gnu.linkonce.sb.*)
		*(.bss .bss.*)
		*(.gnu.linkonce.b.*)
		*(COMMON)

		. = ALIGN(4);
		_ebss = .;
	} >SRAM

	. = ALIGN(8);
	_end = .;

	.stack :
	{
		. = ALIGN(16);
		. += __stack_size;
		_stack_top = .;
	} >SRAM
}
