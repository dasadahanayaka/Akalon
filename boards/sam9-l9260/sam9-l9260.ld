SECTIONS
{
   . = 0x20000000; 
   . = ALIGN(4);
   .text : 
   {
     *(.text) 
   }


   . = ALIGN(4);
   .data : 
   { 
     *(.data) 
   }


   bss_start = . ;
   . = ALIGN(4);
   .bss  : 
   { 
     *(.bss)  
   }
   bss_end = . ;
}
