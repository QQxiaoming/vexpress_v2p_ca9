ROM_SIZE        = 1024*64;
RAM_SIZE        = 1024*64;
C_STACK_SIZE    = 0x2000;
SVC_STACK_SIZE  = 0x1000;
IRQ_STACK_SIZE  = 0x1000;
FIQ_STACK_SIZE  = 0x1000;
UND_STACK_SIZE  = 0x100;
ABT_STACK_SIZE  = 0x100;
HEAP_SIZE       = 0x1000;

USER_SRAM_START_ADDR  = 0x48000000;
USER_SRAM_END_ADDR    = 0x49FFFFFF;
USER_SRAM_SIZE        = 0x02000000;
DDR2_LOW_START_ADDR   = 0x60000000;
DDR2_LOW_END_ADDR     = 0x7FFFFFFF;
DDR2_LOW_SIZE         = 0x20000000;
DDR2_HIGHT_START_ADDR = 0x80000000;
DDR2_HIGHT_END_ADDR   = 0x9FFFFFFF;
DDR2_HIGHT_SIZE       = 0x20000000;

OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(reset)

CSTACK = ORIGIN(RAM) + LENGTH(RAM);
FIQ_STACK = CSTACK - C_STACK_SIZE;
IRQ_STACK = FIQ_STACK - FIQ_STACK_SIZE;
SVC_STACK = IRQ_STACK - IRQ_STACK_SIZE;

_Min_Stack_Size = C_STACK_SIZE + IRQ_STACK_SIZE + FIQ_STACK_SIZE + SVC_STACK_SIZE;

MEMORY
{
	ROM(rx)   : ORIGIN = USER_SRAM_START_ADDR,       LENGTH = ROM_SIZE
	RAM(xrw)  : ORIGIN = ORIGIN(ROM) + LENGTH(ROM) , LENGTH = RAM_SIZE
}

SECTIONS
{
	.text.reset :
	{
		. = ALIGN(4);
		KEEP(*(.text.reset));
		. = ALIGN(4);
	} > ROM

	/* The program code and other data goes into ROM */
	.text :
	{
		. = ALIGN(4);
		*(.text)           /* .text sections (code) */
		*(.text*)          /* .text* sections (code) */
		*(.rodata)         /* .rodata sections (constants, strings, etc.) */
		*(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
		*(.glue_7)         /* glue arm to thumb code */
		*(.glue_7t)        /* glue thumb to arm code */

		KEEP (*(.init))
		KEEP (*(.fini))

		. = ALIGN(4);
		_etext = .;        /* define a global symbols at end of code */
	} > ROM

	.ARM.extab   : 
	{ 
		*(.ARM.extab* .gnu.linkonce.armextab.*) 
	} > ROM

	.ARM : 
	{
		__exidx_start = .;
		*(.ARM.exidx*)
		__exidx_end = .;
	} > ROM

	.ARM.attributes : 
	{ 
		*(.ARM.attributes) 
	} > ROM

	.preinit_array     :
	{
		PROVIDE_HIDDEN (__preinit_array_start = .);
		KEEP (*(.preinit_array*))
		PROVIDE_HIDDEN (__preinit_array_end = .);
	} > ROM

	.init_array :
	{
		PROVIDE_HIDDEN (__init_array_start = .);
		KEEP (*(SORT(.init_array.*)))
		KEEP (*(.init_array*))
		PROVIDE_HIDDEN (__init_array_end = .);
	} > ROM

	.fini_array :
	{
		PROVIDE_HIDDEN (__fini_array_start = .);
		KEEP (*(.fini_array*))
		KEEP (*(SORT(.fini_array.*)))
		PROVIDE_HIDDEN (__fini_array_end = .);
	} > ROM

	/* used by the startup to initialize data */
	.initialize_data :
	{
		PROVIDE(_sidata = .);
	} > ROM

	/* Initialized data sections goes into RAM, load LMA copy after code */
	.data :
	{
		. = ALIGN(4);
		_sdata = .;        /* create a global symbol at data start */
		*(.data)           /* .data sections */
		*(.data*)          /* .data* sections */
		*(.tdata)      
		*(.tdata*)      
		*(.got*)      
		*(.igot*)      

		. = ALIGN(4);
		_edata = .;        /* define a global symbol at data end */
	} > RAM AT > ROM

	/* Uninitialized data section */
	. = ALIGN(4);
	.bss :
	{
		/* This is used by the startup in order to initialize the .bss secion */
		_sbss = .;         /* define a global symbol at bss start */
		__bss_start__ = _sbss;
		*(.bss)
		*(.bss*)
		*(.tbss)
		*(.tbss*)
		*(COMMON)

		. = ALIGN(4);
		_ebss = .;         /* define a global symbol at bss end */
		__bss_end__ = _ebss;
	} > RAM

	/* User_heap_stack section, used to check that there is enough RAM left */
	._user_stack :
	{
		. = ALIGN(4);
		. = . + _Min_Stack_Size;
		. = ALIGN(4);
	} > RAM

	/* Remove information from the standard libraries */
	/DISCARD/ :
	{
		libc.a ( * )
		libm.a ( * )
		libgcc.a ( * )
	}
}
