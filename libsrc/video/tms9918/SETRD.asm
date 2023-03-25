;
; z88dk library: Generic VDP support code
;
; $Id: gen_setrd.asm,v 1.3 2016-06-16 19:30:25 dom Exp $
;

        SECTION code_video_vdp
	PUBLIC	SETRD
	PUBLIC	_SETRD

        EXTERN  l_tms9918_disable_interrupts
        EXTERN  l_tms9918_enable_interrupts

	INCLUDE	"video/tms9918/vdp.inc"

;==============================================================
; VRAM to HL
;==============================================================
; Sets VRAM read address to hl
;
; CORRUPTS bc
;==============================================================
.SETRD
._SETRD
    call    l_tms9918_disable_interrupts
IF VDP_CMD >= 256
    ld      bc,VDP_CMD
ENDIF
IFDEF V9938
    ; High bit of address (bits 14,15,16)
    ld      a,h
    rlca
    rlca
    and     3           ;Ignoring bit 16
    VDPOUT(VDP_CMD)
    ld      a,14 + 0x80
    VDPOUT(VDP_CMD)
ENDIF
    ld      a,l
    VDPOUT(VDP_CMD)
    ld      a,h
    and     $3F
    VDPOUT(VDP_CMD)
    call    l_tms9918_enable_interrupts
    ret
