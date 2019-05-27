
#include "m32_math.h"

extern float m32_coeff_exp[];

float m32_expf (float x) __z88dk_fastcall
{
    int16_t exp;
    uint8_t sign;

    if(x == 0.0)
        return 1.0;
    sign = x < 0.0;
    if(sign)
        x = -x;
    x *= 1.4426950409;        /* convert to log2 */
    exp = (int16_t)m32_floorf(x);
    x -= (float)exp;
    x = m32_ldexpf(m32_polyf(x, m32_coeff_exp, 9), exp);
    if(sign)
        return m32_invf(x);
    return x;
}

