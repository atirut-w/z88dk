;
;       Bondwell 2 pseudo graphics routines
;
;       Stefano Bodrato 2021
;

    SECTION code_graphics

    PUBLIC  w_pixeladdress

    INCLUDE "graphics/grafix.inc"

		;EXTERN   base_graphics

;
;       $Id: w_pixladdr.asm $
;

; ******************************************************************
;
; Get absolute  pixel address in map of virtual (x,y) coordinate.
;
; in:  hl,de    = (x,y) coordinate of pixel
;
; out: hl=de    = address of pixel byte
;          a    = bit number of byte where pixel is to be placed
;         fz    = 1 if bit number is 0 of pixel position
;
; registers changed     after return:
;  ......hl/ixiy same
;  afbcde../.... different
;

w_pixeladdress:
    push    bc

    ld      b, h
    ld      c, l
                                ;calc (y * 80) + (x / 8) + bit map start
    ld      l, e                        ;hl = y
    ld      h, d

    add     hl, hl                      ;hl = y * 16
    add     hl, hl
    add     hl, hl
    add     hl, hl
    ld      d, h
    ld      e, l
    add     hl, hl                      ;hl = y*64
    add     hl, hl

    add     hl, de                      ;hl = (y * 64)+(y * 16)

    ld      e, c                        ;de = x
    ld      d, b

    srl     d                           ;de = x / 8
    rr      e
    srl     d
    rr      e
    srl     d
    rr      e

    add     hl, de                      ;hl = (y * 80) + (x / 8)
    ld      de, 0
    add     hl, de                      ;hl = (y * 80) + (x / 8) + bit map offset

    ld      a, c                        ;a = x low byte
    and     07h                         ;a = x mod 8

    pop     bc

    ld      d, h
    ld      e, l

    xor     7

    ret

