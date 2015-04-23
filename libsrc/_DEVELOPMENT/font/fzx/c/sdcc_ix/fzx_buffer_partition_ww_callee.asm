
; char *fzx_buffer_partition_ww(struct fzx_font *ff, char *buf, uint16_t buflen, uint16_t allowed_width)

SECTION code_font_fzx

PUBLIC _fzx_buffer_partition_ww, l0_fzx_buffer_partition_ww

_fzx_buffer_partition_ww:

   pop hl
   exx
   pop bc
   exx
   pop de
   pop bc
   ex (sp),hl

l0_fzx_buffer_partition_ww:

   exx
   push bc
   exx
   ex (sp),ix
   
   call asm_fzx_buffer_partition_ww
   
   pop ix
   ret
   
   INCLUDE "font/fzx/z80/asm_fzx_buffer_partition_ww.asm"
