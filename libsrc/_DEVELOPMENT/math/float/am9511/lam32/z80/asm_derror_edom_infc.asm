
SECTION code_clib
SECTION code_fp_am9511

PUBLIC derror_edom_infc, derror_edom_pinfc, derror_edom_minfc

EXTERN asm_am9511_derror_edom_infc, asm_am9511_derror_edom_pinfc, asm_am9511_derror_edom_ninfc

defc derror_edom_infc  = asm_am9511_derror_edom_infc
defc derror_edom_pinfc = asm_am9511_derror_edom_pinfc
defc derror_edom_minfc = asm_am9511_derror_edom_ninfc