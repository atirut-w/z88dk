    SECTION code_clib
    PUBLIC  im1_init
    PUBLIC  _im1_init

    EXTERN  asm_im1_handler
    EXTERN  l_push_di
    EXTERN  l_pop_ei


im1_init:
_im1_init:
    call    l_push_di
    ld      hl, asm_im1_handler
    ld      ($fe7a), hl
    ld      a, 195
    ld      ($fe79), a
    call    l_pop_ei
    ret

