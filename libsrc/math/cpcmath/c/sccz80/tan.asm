;
;	CPC Maths Routines
;
;	August 2003 **_|warp6|_** <kbaccam /at/ free.fr>
;
;	$Id: tan.asm,v 1.4 2016-06-22 19:50:49 dom Exp $
;

    SECTION smc_fp
    INCLUDE "cpcmath.inc"

    PUBLIC  tan
    PUBLIC  tanc

    EXTERN  get_para

.tan
    call    get_para
.tanc
    FPCALL(CPCFP_FLO_TAN)
    ret