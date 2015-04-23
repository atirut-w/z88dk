
#ifndef _FONT_FZX_H
#define _FONT_FZX_H

#include <stddef.h>
#include <stdint.h>
#include <rect.h>

// DATA STRUCTURES

struct fzx_font
{
   uint8_t   height;                   // pixel height of font
   uint8_t   tracking;                 // horizontal gap between chars in pixels
   uint8_t   last_char;                // last defined char in bitmap 32..255
};

struct fzx_char
{
   uint16_t  kern_offset;              // kern in bits 15..14, offset to char bitmap
   uint8_t   shift_width_1;            // vertical shift in bits 7..4, width-1 in bits 3..0
};

struct fzx_cmetric
{
   uint8_t   kern;                     // num pixels to leftshift rendered char, 0-3
   uint8_t   width;                    // width of glyph, 1-16
   uint8_t   shift;                    // num pixels to downshift rendered char from top of line, 0-15
   uint8_t  *bitmap;                   // address of glyph bitmap
};

struct fzx_state
{
   uint8_t          jp;                // 195 = z80 jump instruction
   void            *fzx_draw;          // address of fzx_draw function
   struct fzx_font *font;              // selected font
   uint16_t         x;                 // x coordinate in pixels
   uint16_t         y;                 // y coordinate in pixels
   struct r_Rect16  paper;             // display area units in pixels
   uint16_t         left_margin;       // left margin in pixels
   uint8_t          space_expand;      // addition width added to space characters
   uint16_t         reserved;          // unused
   
   #ifdef __SPECTRUM
   
   // zx spectrum only below
   
   uint8_t          fgnd_attr;         // text colour
   uint8_t          fgnd_mask;         // set bits indicate kept background attribute bits

   #endif
};

// FZX DRAW MODES

extern void _fzx_draw_or(void);        // OR fzx pixels into display
extern void _fzx_draw_reset(void);     // CLEAR display where fzx pixels are set
extern void _fzx_draw_xor(void);       // XOR fzx pixels into display

// FUNCTIONS

#ifdef __SDCC

// SDCC

extern void      fzx_state_init(struct fzx_state *fs, struct fzx_font *ff, struct r_Rect16 *window);

extern void      fzx_at(struct fzx_state *fs, uint16_t x, uint16_t y);
extern int       fzx_putc(struct fzx_state *fs, int c);

extern char     *fzx_char_metrics(struct fzx_font *ff, struct fzx_cmetric *fm, int c);
extern uint16_t  fzx_glyph_width(struct fzx_font *ff, int c);

extern uint16_t  fzx_string_extent(struct fzx_font *ff, char *s);
extern char     *fzx_string_partition(struct fzx_font *ff, char *s, uint16_t allowed_width);
extern char     *fzx_string_partition_ww(struct fzx_font *ff, char *s, uint16_t allowed_width);

extern uint16_t  fzx_buffer_extent(struct fzx_font *ff, char *buf, uint16_t buflen);
extern char     *fzx_buffer_partition(struct fzx_font *ff, char *buf, uint16_t buflen, uint16_t allowed_width);
extern char     *fzx_buffer_partition_ww(struct fzx_font *ff, char *buf, uint16_t buflen, uint16_t allowed_width);

extern int       fzx_puts(struct fzx_state *fs, char *s);
extern int       fzx_puts_justified(struct fzx_state *fs, char *s, uint16_t allowed_width);

extern int       fzx_write(struct fzx_state *fs, char *buf, uint16_t buflen);
extern int       fzx_write_justified(struct fzx_state *fs, char *buf, uint16_t buflen, uint16_t allowed_width);

#ifndef __SDCC_DISABLE_CALLEE

// SDCC CALLEE LINKAGE

extern void      fzx_state_init_callee(struct fzx_state *fs, struct fzx_font *ff, struct r_Rect16 *window) __z88dk_callee;

extern void      fzx_at_callee(struct fzx_state *fs, uint16_t x, uint16_t y) __z88dk_callee;
extern int       fzx_putc_callee(struct fzx_state *fs, int c) __z88dk_callee;

extern char     *fzx_char_metrics_callee(struct fzx_font *ff, struct fzx_cmetric *fm, int c) __z88dk_callee;
extern uint16_t  fzx_glyph_width_callee(struct fzx_font *ff, int c) __z88dk_callee;

extern uint16_t  fzx_string_extent_callee(struct fzx_font *ff, char *s) __z88dk_callee;
extern char     *fzx_string_partition_callee(struct fzx_font *ff, char *s, uint16_t allowed_width) __z88dk_callee;
extern char     *fzx_string_partition_ww_callee(struct fzx_font *ff, char *s, uint16_t allowed_width) __z88dk_callee;

extern uint16_t  fzx_buffer_extent_callee(struct fzx_font *ff, char *buf, uint16_t buflen) __z88dk_callee;
extern char     *fzx_buffer_partition_callee(struct fzx_font *ff, char *buf, uint16_t buflen, uint16_t allowed_width) __z88dk_callee;
extern char     *fzx_buffer_partition_ww_callee(struct fzx_font *ff, char *buf, uint16_t buflen, uint16_t allowed_width) __z88dk_callee;

extern int       fzx_puts_callee(struct fzx_state *fs, char *s) __z88dk_callee;
extern int       fzx_puts_justified_callee(struct fzx_state *fs, char *s, uint16_t allowed_width) __z88dk_callee;

extern int       fzx_write_callee(struct fzx_state *fs, char *buf, uint16_t buflen) __z88dk_callee;
extern int       fzx_write_justified_callee(struct fzx_state *fs, char *buf, uint16_t buflen, uint16_t allowed_width) __z88dk_callee;

// SDCC MAKE CALLEE LINKAGE THE DEFAULT

#define fzx_state_init(a,b,c)               fzx_state_init_callee(a,b,c)
#define fzx_at(a,b,c)                       fzx_at_callee(a,b,c)
#define fzx_putc(a,b)                       fzx_putc_callee(a,b)
#define fzx_char_metrics(a,b,c)             fzx_char_metrics_callee(a,b,c)
#define fzx_glyph_width(a,b)                fzx_glyph_width_callee(a,b)
#define fzx_string_extent(a,b)              fzx_string_extent_callee(a,b)
#define fzx_string_partition(a,b,c)         fzx_string_partition_callee(a,b,c)
#define fzx_string_partition_ww(a,b,c)      fzx_string_partition_ww_callee(a,b,c)
#define fzx_buffer_extent(a,b,c)            fzx_buffer_extent_callee(a,b,c)
#define fzx_buffer_partition(a,b,c,d)       fzx_buffer_partition_callee(a,b,c,d)
#define fzx_buffer_partition_ww(a,b,c,d)    fzx_buffer_partition_ww_callee(a,b,c,d)
#define fzx_puts(a,b)                       fzx_puts_callee(a,b)
#define fzx_puts_justified(a,b,c)           fzx_puts_justified_callee(a,b,c)
#define fzx_write(a,b,c)                    fzx_write_callee(a,b,c)
#define fzx_write_justified(a,b,c,d)        fzx_write_justified_callee(a,b,c,d)

#endif

#else

// SCCZ80

extern void     __LIB__             fzx_state_init(struct fzx_state *fs, struct fzx_font *ff, struct r_Rect16 *window);

extern void     __LIB__             fzx_at(struct fzx_state *fs, uint16_t x, uint16_t y);
extern int      __LIB__             fzx_putc(struct fzx_state *fs, int c);

extern char     __LIB__            *fzx_char_metrics(struct fzx_font *ff, struct fzx_cmetric *fm, int c);
extern uint16_t __LIB__             fzx_glyph_width(struct fzx_font *ff, int c);

extern uint16_t __LIB__             fzx_string_extent(struct fzx_font *ff, char *s);
extern char     __LIB__            *fzx_string_partition(struct fzx_font *ff, char *s, uint16_t allowed_width);
extern char     __LIB__            *fzx_string_partition_ww(struct fzx_font *ff, char *s, uint16_t allowed_width);

extern uint16_t __LIB__             fzx_buffer_extent(struct fzx_font *ff, char *buf, uint16_t buflen);
extern char     __LIB__            *fzx_buffer_partition(struct fzx_font *ff, char *buf, uint16_t buflen, uint16_t allowed_width);
extern char     __LIB__            *fzx_buffer_partition_ww(struct fzx_font *ff, char *buf, uint16_t buflen, uint16_t allowed_width);

extern int      __LIB__             fzx_puts(struct fzx_state *fs, char *s);
extern int      __LIB__             fzx_puts_justified(struct fzx_state *fs, char *s, uint16_t allowed_width);

extern int      __LIB__             fzx_write(struct fzx_state *fs, char *buf, uint16_t buflen);
extern int      __LIB__             fzx_write_justified(struct fzx_state *fs, char *buf, uint16_t buflen, uint16_t allowed_width);

// SCCZ80 CALLEE LINKAGE

extern void     __LIB__ __CALLEE__  fzx_state_init_callee(struct fzx_state *fs, struct fzx_font *ff, struct r_Rect16 *window);

extern void     __LIB__ __CALLEE__  fzx_at_callee(struct fzx_state *fs, uint16_t x, uint16_t y);
extern int      __LIB__ __CALLEE__  fzx_putc_callee(struct fzx_state *fs, int c);

extern char     __LIB__ __CALLEE__ *fzx_char_metrics_callee(struct fzx_font *ff, struct fzx_cmetric *fm, int c);
extern uint16_t __LIB__ __CALLEE__  fzx_glyph_width_callee(struct fzx_font *ff, int c);

extern uint16_t __LIB__ __CALLEE__  fzx_string_extent_callee(struct fzx_font *ff, char *s);
extern char     __LIB__ __CALLEE__ *fzx_string_partition_callee(struct fzx_font *ff, char *s, uint16_t allowed_width);
extern char     __LIB__ __CALLEE__ *fzx_string_partition_ww_callee(struct fzx_font *ff, char *s, uint16_t allowed_width);

extern uint16_t __LIB__ __CALLEE__  fzx_buffer_extent_callee(struct fzx_font *ff, char *buf, uint16_t buflen);
extern char     __LIB__ __CALLEE__ *fzx_buffer_partition_callee(struct fzx_font *ff, char *buf, uint16_t buflen, uint16_t allowed_width);
extern char     __LIB__ __CALLEE__ *fzx_buffer_partition_ww_callee(struct fzx_font *ff, char *buf, uint16_t buflen, uint16_t allowed_width);

extern int      __LIB__ __CALLEE__  fzx_puts_callee(struct fzx_state *fs, char *s);
extern int      __LIB__ __CALLEE__  fzx_puts_justified_callee(struct fzx_state *fs, char *s, uint16_t allowed_width);

extern int      __LIB__ __CALLEE__  fzx_write_callee(struct fzx_state *fs, char *buf, uint16_t buflen);
extern int      __LIB__ __CALLEE__  fzx_write_justified_callee(struct fzx_state *fs, char *buf, uint16_t buflen, uint16_t allowed_width);

// SCCZ80 MAKE CALLEE LINKAGE THE DEFAULT

#define fzx_state_init(a,b,c)               fzx_state_init_callee(a,b,c)
#define fzx_at(a,b,c)                       fzx_at_callee(a,b,c)
#define fzx_putc(a,b)                       fzx_putc_callee(a,b)
#define fzx_char_metrics(a,b,c)             fzx_char_metrics_callee(a,b,c)
#define fzx_glyph_width(a,b)                fzx_glyph_width_callee(a,b)
#define fzx_string_extent(a,b)              fzx_string_extent_callee(a,b)
#define fzx_string_partition(a,b,c)         fzx_string_partition_callee(a,b,c)
#define fzx_string_partition_ww(a,b,c)      fzx_string_partition_ww_callee(a,b,c)
#define fzx_buffer_extent(a,b,c)            fzx_buffer_extent_callee(a,b,c)
#define fzx_buffer_partition(a,b,c,d)       fzx_buffer_partition_callee(a,b,c,d)
#define fzx_buffer_partition_ww(a,b,c,d)    fzx_buffer_partition_ww_callee(a,b,c,d)
#define fzx_puts(a,b)                       fzx_puts_callee(a,b)
#define fzx_puts_justified(a,b,c)           fzx_puts_justified_callee(a,b,c)
#define fzx_write(a,b,c)                    fzx_write_callee(a,b,c)
#define fzx_write_justified(a,b,c,d)        fzx_write_justified_callee(a,b,c,d)

#endif


// FZX FONTS BY ANDREW OWEN, LATIN VARIANTS BY TK90X

extern struct fzx_font ff_ao_Aribeth14;
extern struct fzx_font ff_ao_Aribeth6;
extern struct fzx_font ff_ao_Aribeth8;
extern struct fzx_font ff_ao_Arkade;
extern struct fzx_font ff_ao_Blackadder;
extern struct fzx_font ff_ao_Cayeux;
extern struct fzx_font ff_ao_Cheveron;
extern struct fzx_font ff_ao_Chloe;
extern struct fzx_font ff_ao_Dutch;
extern struct fzx_font ff_ao_DutchLatin1;
extern struct fzx_font ff_ao_DutchLatin5;
extern struct fzx_font ff_ao_GenevaMonoBold;
extern struct fzx_font ff_ao_GenevaMonoBoldItalic;
extern struct fzx_font ff_ao_GenevaMonoCyrillic;
extern struct fzx_font ff_ao_GenevaMonoItalic;
extern struct fzx_font ff_ao_GenevaMonoRoman;
extern struct fzx_font ff_ao_GenevaMonoSymbol;
extern struct fzx_font ff_ao_Grotesk;
extern struct fzx_font ff_ao_GroteskLatin1;
extern struct fzx_font ff_ao_GroteskLatin5;
extern struct fzx_font ff_ao_Hemery;
extern struct fzx_font ff_ao_Hiroshi;
extern struct fzx_font ff_ao_Jangeborg;
extern struct fzx_font ff_ao_Kaitaia;
extern struct fzx_font ff_ao_Klausjahn;
extern struct fzx_font ff_ao_Lettera;
extern struct fzx_font ff_ao_Napier;
extern struct fzx_font ff_ao_Newbrand;
extern struct fzx_font ff_ao_OldEnglish;
extern struct fzx_font ff_ao_OpenDyslexic;
extern struct fzx_font ff_ao_Orion;
extern struct fzx_font ff_ao_Prefect;
extern struct fzx_font ff_ao_Programmer;
extern struct fzx_font ff_ao_RoundelSans;
extern struct fzx_font ff_ao_RoundelSansLatin1;
extern struct fzx_font ff_ao_RoundelSansLatin5;
extern struct fzx_font ff_ao_RoundelSerif;
extern struct fzx_font ff_ao_RoundelSerifLatin1;
extern struct fzx_font ff_ao_RoundelSerifLatin5;
extern struct fzx_font ff_ao_Sabon;
extern struct fzx_font ff_ao_Sinclair;
extern struct fzx_font ff_ao_SinclairLatin1;
extern struct fzx_font ff_ao_SinclairLatin5;
extern struct fzx_font ff_ao_SoixanteQuatre;
extern struct fzx_font ff_ao_Soxz;
extern struct fzx_font ff_ao_SpecDings;
extern struct fzx_font ff_ao_Swiss;
extern struct fzx_font ff_ao_Trader;
extern struct fzx_font ff_ao_Twombly;
extern struct fzx_font ff_ao_Vickers;
extern struct fzx_font ff_ao_Videotex;
extern struct fzx_font ff_ao_Zaibatsu;
extern struct fzx_font ff_ao_ZaibatsuLatin1;
extern struct fzx_font ff_ao_ZaibatsuLatin5;


// FZX FONTS FROM KLUB UZIVATELU DESKTOPU
// http://www.worldofspectrum.org/infoseekid.cgi?id=0023578

extern struct fzx_font ff_dkud1_Beerland2;
extern struct fzx_font ff_dkud1_Cinema;
extern struct fzx_font ff_dkud1_Cobretti;
extern struct fzx_font ff_dkud1_Consul1;
extern struct fzx_font ff_dkud1_Consul2;
extern struct fzx_font ff_dkud1_Crusade;
extern struct fzx_font ff_dkud1_DoubleBold;
extern struct fzx_font ff_dkud1_Edice13;
extern struct fzx_font ff_dkud1_JmenoRuze;
extern struct fzx_font ff_dkud1_Kurziva;
extern struct fzx_font ff_dkud1_Little;
extern struct fzx_font ff_dkud1_MahJongg;
extern struct fzx_font ff_dkud1_Overline;
extern struct fzx_font ff_dkud1_Phones2;
extern struct fzx_font ff_dkud1_RightBold;
extern struct fzx_font ff_dkud1_SciFi;
extern struct fzx_font ff_dkud1_Sinclair;
extern struct fzx_font ff_dkud1_Subtitles;
extern struct fzx_font ff_dkud1_Transcript;
extern struct fzx_font ff_dkud1_UpnUp1;
extern struct fzx_font ff_dkud1_UpnUp2;


// FZX FONTS FROM UNIVERSUM'S DESKTOP
// http://www.worldofspectrum.org/infoseekid.cgi?id=0016682

extern struct fzx_font ff_dkud2_Academy;
extern struct fzx_font ff_dkud2_Belegost1;
extern struct fzx_font ff_dkud2_Belegost2;
extern struct fzx_font ff_dkud2_BigBold;
extern struct fzx_font ff_dkud2_UltraBold;
extern struct fzx_font ff_dkud2_UpcaseBold;
extern struct fzx_font ff_dkud2_Cobra;
extern struct fzx_font ff_dkud2_Crash;
extern struct fzx_font ff_dkud2_doc;
extern struct fzx_font ff_dkud2_Eclipse;
extern struct fzx_font ff_dkud2_Extra;
extern struct fzx_font ff_dkud2_Hijack;
extern struct fzx_font ff_dkud2_Italika;
extern struct fzx_font ff_dkud2_Just6;
extern struct fzx_font ff_dkud2_Locomotion;
extern struct fzx_font ff_dkud2_Midnight;
extern struct fzx_font ff_dkud2_MoonAlert;
extern struct fzx_font ff_dkud2_Nether;
extern struct fzx_font ff_dkud2_Neverend;
extern struct fzx_font ff_dkud2_Roman;
extern struct fzx_font ff_dkud2_Script1;
extern struct fzx_font ff_dkud2_Script2;
extern struct fzx_font ff_dkud2_Standard;
extern struct fzx_font ff_dkud2_Tomahawk;
extern struct fzx_font ff_dkud2_Wildvest;
extern struct fzx_font ff_dkud2_Winter;


// FZX FONTS FROM KLUB UZIVATELU DESKTOPU 3
// http://www.worldofspectrum.org/infoseekid.cgi?id=0023577

extern struct fzx_font ff_dkud3_Azbuka;
extern struct fzx_font ff_dkud3_AzbukaBold;
extern struct fzx_font ff_dkud3_Belegost3;
extern struct fzx_font ff_dkud3_CobraLight;
extern struct fzx_font ff_dkud3_CobraTall;
extern struct fzx_font ff_dkud3_DeutchBold;
extern struct fzx_font ff_dkud3_TrnsDeutch;
extern struct fzx_font ff_dkud3_Echo;
extern struct fzx_font ff_dkud3_Erotic;
extern struct fzx_font ff_dkud3_Lovely;
extern struct fzx_font ff_dkud3_NisaSport1;
extern struct fzx_font ff_dkud3_NisaSport2;
extern struct fzx_font ff_dkud3_Notes;
extern struct fzx_font ff_dkud3_Pearl;
extern struct fzx_font ff_dkud3_Pipe;
extern struct fzx_font ff_dkud3_PipePlus;
extern struct fzx_font ff_dkud3_QuaUni;
extern struct fzx_font ff_dkud3_Script;
extern struct fzx_font ff_dkud3_Small1;
extern struct fzx_font ff_dkud3_Small2;
extern struct fzx_font ff_dkud3_SteAff;
extern struct fzx_font ff_dkud3_Times1;
extern struct fzx_font ff_dkud3_Times2;
extern struct fzx_font ff_dkud3_Western;
extern struct fzx_font ff_dkud3_Windows;
extern struct fzx_font ff_dkud3_WingCmdr1;
extern struct fzx_font ff_dkud3_WingCmdr2;


// FZX FONTS FROM DESKTOP FONT EDITOR
// http://www.worldofspectrum.org/infoseekid.cgi?id=0016682

extern struct fzx_font ff_dkud4_Font01;
extern struct fzx_font ff_dkud4_Font02;
extern struct fzx_font ff_dkud4_Font03;
extern struct fzx_font ff_dkud4_Font04;
extern struct fzx_font ff_dkud4_Font05;
extern struct fzx_font ff_dkud4_Font06;
extern struct fzx_font ff_dkud4_Font07;
extern struct fzx_font ff_dkud4_Font08;
extern struct fzx_font ff_dkud4_Font09;
extern struct fzx_font ff_dkud4_Font10;
extern struct fzx_font ff_dkud4_Font11;
extern struct fzx_font ff_dkud4_Font12;
extern struct fzx_font ff_dkud4_Font13;
extern struct fzx_font ff_dkud4_Font14;
extern struct fzx_font ff_dkud4_Font15;


// FZX FONTS BY INDEPENDENT DESIGNERS KWEEPA & TK90X

extern struct fzx_font ff_ind_Termino;
extern struct fzx_font ff_ind_TerminoLatin1;
extern struct fzx_font ff_ind_TerminoLatin5;
extern struct fzx_font ff_ind_vga;


// FZX FONTS BY UTZ

extern struct fzx_font ff_utz_1stGrade;
extern struct fzx_font ff_utz_Blokz;
extern struct fzx_font ff_utz_GridDrop0;
extern struct fzx_font ff_utz_GridDrop1;
extern struct fzx_font ff_utz_Handpress;
extern struct fzx_font ff_utz_KickDrop;
extern struct fzx_font ff_utz_Notez;
extern struct fzx_font ff_utz_PhatKidzBRK;
extern struct fzx_font ff_utz_Phraktur;
extern struct fzx_font ff_utz_SkoolBrkCD;
extern struct fzx_font ff_utz_SkoolBrkEXT;
extern struct fzx_font ff_utz_SkoolBrkU;
extern struct fzx_font ff_utz_ShadowFlower;
extern struct fzx_font ff_utz_Skyscrap;
extern struct fzx_font ff_utz_TinyTexanS;
extern struct fzx_font ff_utz_TinyTexanXS;
extern struct fzx_font ff_utz_TinyTexanXXS;

#endif
