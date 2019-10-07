#define OP_HLT               0x00
#define OP_NOT               0x01
#define OP_MTC               0x02
#define OP_BOL               0x04
#define OP_NOP               0x08
#define OP_HDR_END           0x0C
#define OP_ENC               0x10
#define OP_ENC_UTF8          0x10
#define OP_ENC_LATIN1        0x11
#define OP_ENC_LATIN2        0x12
#define OP_ENC_LATIN3        0x13
#define OP_ENC_LATIN4        0x14
#define OP_ENC_LATIN5        0x15
#define OP_ENC_LATIN6        0x16
#define OP_ENC_LATIN7        0x17
#define OP_ENC_LATIN8        0x18
#define OP_ENC_LATIN9        0x19
#define OP_ENC_LATIN10       0x1A
#define OP_CSI               0x1C
#define OP_CSS               0x1D
#define OP_ISA               0x20
#define OP_ISA_ANY           0x20
#define OP_ISA_NOTEOL        0x21
#define OP_ISA_EOL           0x22
#define OP_ISA_UPPER         0x23
#define OP_ISA_LOWER         0x24
#define OP_ISA_ALPHA         0x25
#define OP_ISA_ALNUM         0x26
#define OP_ISA_PUNCT         0x27
#define OP_ISA_CTRL          0x28
#define OP_ISA_DIGIT         0x29
#define OP_ISA_HEXDIGIT      0x2A
#define OP_ISA_SPACE         0x2B
#define OP_ISA_BLANK         0x2C
#define OP_ISA_WORDCHAR      0x2D
#define OP_ISA_INT           0x30
#define OP_ISA_HEXINT        0x31
#define OP_ISA_FLOAT         0x32
#define OP_ISA_BLANKSEQ      0x33
#define OP_ISA_WORD          0x34
#define OP_ISA_SIGN          0x35
#define OP_ISA_BALANCED      0x36
#define OP_ISA_QSTRING       0x37
#define OP_BGN               0x40
#define OP_END               0x50
#define OP_ISN               0x60
#define OP_FOR_STAR          0x80
#define OP_FOR_PLUS          0x81
#define OP_STO               0x82
#define OP_RCL               0x83
#define OP_RST               0x84
#define OP_ONF               0x90
#define OP_JPM               0xA0
#define OP_JPF               0xB0
#define OP_NXT               0xC0
#define OP_FOR               0xD0

#define OP_CHR_BASE          0xE0
#define OP_CHR_U_BASE        0xF0
#define OP_RNG_BASE          0xE8
#define OP_CHH3              0xE2
#define OP_CHH4              0xF2

#define OP_CHR_FOLD          0xE0
#define OP_CHN_FOLD          0xE1
#define OP_RNG_FOLD          0xE8
#define OP_RGN_FOLD          0xE9
#define OP_CHR               0xE4
#define OP_CHN               0xE5
#define OP_RNG               0xEC
#define OP_RGN               0xED
#define OP_CHR_U_FOLD        0xF0
#define OP_CHN_U_FOLD        0xF1
#define OP_RNG_U_FOLD        0xF8
#define OP_RGN_U_FOLD        0xF9
#define OP_CHR_U             0xF4
#define OP_CHN_U             0xF5
#define OP_RNG_U             0xFC
#define OP_RGN_U             0xFD

#define OP_HDR_START         0xFF
#define OP_HDR_NCAPT         0xEF

extern uint8_t skpoplen_[16];

#define skpoplen(x) (skpoplen_[((x)>>4) & 0x0F])
