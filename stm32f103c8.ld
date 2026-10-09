ENTRY(Reset_Handler)
_estack = 0x20005000;
MEMORY {
  FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 64K
  RAM   (xrw) : ORIGIN = 0x20000000, LENGTH = 20K
}
SECTIONS {
  .isr_vector : { KEEP(*(.isr_vector)) } >FLASH
  .text : { *(.text*) *(.rodata*) KEEP(*(.init)) KEEP(*(.fini)) . = ALIGN(4); } >FLASH
  .preinit_array : { __preinit_array_start = .; KEEP(*(.preinit_array*)) __preinit_array_end = .; } >FLASH
  .init_array : { __init_array_start = .; KEEP(*(.init_array*)) __init_array_end = .; } >FLASH
  .fini_array : { __fini_array_start = .; KEEP(*(.fini_array*)) __fini_array_end = .; } >FLASH
  _sidata = LOADADDR(.data);
  .data : { _sdata = .; *(.data*) . = ALIGN(4); _edata = .; } >RAM AT>FLASH
  .bss : { _sbss = .; *(.bss*) *(COMMON) . = ALIGN(4); _ebss = .; } >RAM
  PROVIDE(end = .);
}
