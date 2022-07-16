OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY( _start ) 

MEMORY 
{ 
	rom (rxai!w) : ORIGIN = 0x00001000, LENGTH = 512k 
}

SECTIONS
{
  .text :
  {
    KEEP(*(.text))  
  } >rom 
}