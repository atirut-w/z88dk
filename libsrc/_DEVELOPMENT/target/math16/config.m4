divert(-1)

###############################################################
# MASTER CONFIGURATION FILE
# user edits are applied to config_*.m4 files in the directory

ifdef(`CFG_ASM_PUB', `define(`CFG_ASM_DEF', 1)')

###############################################################

divert(0)

dnl############################################################
dnl# BEGIN IF GUARDS
dnl############################################################

ifdef(`CFG_ASM_DEF',
`
IFNDEF __CONFIG_Z88DK_MATH16_INC_
defc   __CONFIG_Z88DK_MATH16_INC_ = 1

; Automatically Generated at Library Build Time

')

ifdef(`CFG_C_DEF',
`
#ifndef __CONFIG_Z88DK_MATH16_H_
#define __CONFIG_Z88DK_MATH16_H_

// Automatically Generated at Library Build Time

')

dnl############################################################
dnl# INCLUDE ALL CONFIG FILES
dnl############################################################


dnl############################################################
dnl# END IF GUARDS
dnl############################################################

ifdef(`CFG_C_DEF',
`
#endif
')

ifdef(`CFG_ASM_DEF',
`
ENDIF
')
