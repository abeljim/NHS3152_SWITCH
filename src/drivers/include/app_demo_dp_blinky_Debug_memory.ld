/*
 * GENERATED FILE - DO NOT EDIT
 * (c) Code Red Technologies Ltd, 2008-2013
 * (c) NXP Semiconductors 2013-2022
 * Generated linker script file for NHS3152
 * Created from memory.ldt by FMCreateLinkMemory
 * Using Freemarker v2.3.23
 * MCUXpresso IDE v10.2.1 [Build 795] [2018-07-25]  on Nov 8, 2022 6:09:09 PM
 */

MEMORY
{
  /* Define each memory region */
  Flash30 (rx) : ORIGIN = 0x0, LENGTH = 0x7800 /* 30K bytes (alias Flash) */  
  SRAM8 (rwx) : ORIGIN = 0x10000000, LENGTH = 0x1ff0 /* 8176 bytes (alias RAM) */  
}

  /* Define a symbol for the top of each memory region */
  __base_Flash30 = 0x0  ; /* Flash30 */  
  __base_Flash = 0x0 ; /* Flash */  
  __top_Flash30 = 0x0 + 0x7800 ; /* 30K bytes */  
  __top_Flash = 0x0 + 0x7800 ; /* 30K bytes */  
  __base_SRAM8 = 0x10000000  ; /* SRAM8 */  
  __base_RAM = 0x10000000 ; /* RAM */  
  __top_SRAM8 = 0x10000000 + 0x1ff0 ; /* 8176 bytes */  
  __top_RAM = 0x10000000 + 0x1ff0 ; /* 8176 bytes */  
