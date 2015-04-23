
; void fzx_at_callee(struct fzx_state *fs, uint16_t x, uint16_t y)

SECTION code_font_fzx

PUBLIC _fzx_at_callee, l0_fzx_at_callee

_fzx_at_callee:

   pop af
   pop de
   pop hl
   pop bc
   push af

l0_fzx_at_callee:

   push de
   ex (sp),ix
   
   call asm_fzx_at
   
   pop ix
   ret
   
   INCLUDE "font/fzx/z80/asm_fzx_at.asm"
