;
;	This file is automatically generated
;
;	Do not edit!!!
;
;	djm 12/2/2000
;
;	ZSock Lib function: inet_ntoa

    SECTION code_clib
    PUBLIC  inet_ntoa
    PUBLIC  _inet_ntoa

    EXTERN  no_zsock

    INCLUDE "packages.def"
    INCLUDE "zsock.def"

inet_ntoa:
_inet_ntoa:
    ld      a, r_inet_ntoa
    call_pkg    (tcp_all)
    ret     nc
; We failed..are we installed?
    cp      rc_pnf
    scf                                 ;signal error
    ret     nz                          ;Internal error
    call_pkg    (tcp_ayt)
    jr      nc, inet_ntoa
    jp      no_zsock
