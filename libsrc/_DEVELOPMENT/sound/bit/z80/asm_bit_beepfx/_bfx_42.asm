
; BeepFX sound effect by shiru
; http://shiru.untergrund.net

XLIB _bfx_42

_bfx_42:

   ; Select_3

   defb 1 ;tone
   defw 4,1000,500,100,384
   defb 1 ;tone
   defw 4,1000,500,100,64
   defb 1 ;tone
   defw 4,1000,500,100,16
   defb 0
