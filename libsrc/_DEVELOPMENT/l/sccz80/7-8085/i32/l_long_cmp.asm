;       Z88 Small C+ Run Time Library 
;       Long support functions
;       "8080" mode
;       Stefano - 30/4/2002
;
;       feilipu 10/2021
;       8085 optimisation

SECTION code_clib
SECTION code_l_sccz80

PUBLIC l_long_cmp

; Signed compare of primary dehl (stack) and secondary dehl (registers)
;
; Entry:    primary  = (under two return addresses on stack)
;           secondary= dehl
;
; Exit:     Z = numbers the same
;           NZ = numbers different
;           C/NC = sign of difference [set if secondary > primary]
;           hl = 1
;
; Code takes secondary from primary

.l_long_cmp
    ld      bc,de       ;get the upper 16 into bc
    ld      de,sp+4     ;points to i32 on stack

    ld      a,(de)
    sub     a,l
    ld      l,a

    inc     de

    ld      a,(de)
    sbc     a,h
    ld      h,a

    inc     de

    ld      a,(de)
    sbc     a,c
    ld      c,a

    inc     de

    ld      a,(de)
    sbc     a,b
    ld      b,a

; ATP we have done the comparision and are left with bchl = result of
; primary - secondary, if we have a negative sign then secondary > primary

    jp      M,l_long_cmp1

; Primary was larger, return NC
    ld      a,l
    or      h
    or      c
    or      b
    scf
    ccf
    jp      l_long_cmp2

; Secondary was larger, return C
.l_long_cmp1
    ld      a,l
    or      h
    or      c
    or      b
    scf

; We need to preserve flags
.l_long_cmp2
    pop     bc          ;get returns
    pop     de
    inc     sp          ;i32
    inc     sp
    inc     sp
    inc     sp
    push    de          ;save returns
    push    bc

    ld      hl,1        ;saves some mem in comparision unfunctions
    ret
