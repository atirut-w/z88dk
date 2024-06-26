;       Z88 Small C+ Run time Library
;       l_gint variant to be used sometimes by the peephole optimizer
;

SECTION code_clib
SECTION code_l_sccz80

PUBLIC l_g2intspsp

l_g2intspsp:
    add     hl,sp
    ld      bc,hl
    inc     hl
    inc     hl
    ld      a,(hl)
    inc     hl
    ld      h,(hl)
    ld      l,a
    ex      (sp),hl
    push    hl
    ld      hl,bc
    ld      a,(hl)
    inc     hl
    ld      h,(hl)
    ld      l,a
    ex      (sp),hl
    jp      (hl)

