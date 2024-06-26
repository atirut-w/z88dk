
    MODULE  generic_console_ioctl
    PUBLIC  generic_console_ioctl

    SECTION code_clib

    EXTERN  generic_console_cls
    EXTERN  generic_console_setup_mode
    EXTERN  gr_defmod
    EXTERN  generic_console_font32
    EXTERN  generic_console_udg32

    INCLUDE "ioctl.def"

    PUBLIC  CLIB_GENCON_CAPS
    defc    CLIB_GENCON_CAPS=CAP_GENCON_FG_COLOUR|CAP_GENCON_BG_COLOUR|CAP_GENCON_INVERSE|CAP_GENCON_CUSTOM_FONT|CAP_GENCON_UDGS



; a = ioctl
; de = arg
generic_console_ioctl:
    ex      de, hl
    ld      c, (hl)                     ;bc = where we point to
    inc     hl
    ld      b, (hl)
    cp      IOCTL_GENCON_SET_FONT32
    jr      nz, check_set_udg
    ld      (generic_console_font32), bc
success:
    and     a
    ret
check_set_udg:
    cp      IOCTL_GENCON_SET_UDGS
    jr      nz, check_mode
    ld      (generic_console_udg32), bc
    jr      success
check_mode:
    cp      IOCTL_GENCON_SET_MODE
    jr      nz, failure
    ld      a, c                        ; The mode
    cp      4
    jr      nc, failure
    and     a
    jr      z, failure
    ld      l, a
    push    af
    call    gr_defmod
    pop     af
    call    generic_console_setup_mode
    call    generic_console_cls
    and     a
    ret
failure:
    scf
    ret
