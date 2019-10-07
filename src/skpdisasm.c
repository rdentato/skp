#include "skp.h"
#include "skpopcodes.h"

// ***************************************************************************
// DISASSEMBLE ***************************************************************
// ***************************************************************************
static char *opcode_str(uint8_t op)
{
  switch(op) {
    case OP_HLT                       :return("HLT");
    case OP_NOT                       :return("NOT");
    case OP_MTC                       :return("MTC");
    case OP_BOL                       :return("BOL");
    case OP_NOP                       :return("NOP");
    case OP_ENC_UTF8                  :return("ENC UTF8");
    case OP_ENC_LATIN1                :return("ENC LATIN1");
    case OP_ENC_LATIN2                :return("ENC LATIN2");
    case OP_ENC_LATIN3                :return("ENC LATIN3");
    case OP_ENC_LATIN4                :return("ENC LATIN4");
    case OP_ENC_LATIN5                :return("ENC LATIN5");
    case OP_ENC_LATIN6                :return("ENC LATIN6");
    case OP_ENC_LATIN7                :return("ENC LATIN7");
    case OP_ENC_LATIN8                :return("ENC LATIN8");
    case OP_ENC_LATIN9                :return("ENC LATIN9");
    case OP_ENC_LATIN10               :return("ENC LATIN10");
    case OP_CSS                       :return("CSS");
    case OP_CSI                       :return("CSI");
    case OP_ISA_ANY                   :return("ISA ANY");
    case OP_ISA_NOTEOL                :return("ISA NOTEOL");
    case OP_ISA_EOL                   :return("ISA EOL");
    case OP_ISA_UPPER                 :return("ISA UPPER");
    case OP_ISA_LOWER                 :return("ISA LOWER");
    case OP_ISA_ALPHA                 :return("ISA ALPHA");
    case OP_ISA_ALNUM                 :return("ISA ALNUM");
    case OP_ISA_PUNCT                 :return("ISA PUNCT");
    case OP_ISA_CTRL                  :return("ISA CTRL");
    case OP_ISA_DIGIT                 :return("ISA DIGIT");
    case OP_ISA_HEXDIGIT              :return("ISA HEXDIGIT");
    case OP_ISA_SPACE                 :return("ISA SPACE");
    case OP_ISA_BLANK                 :return("ISA BLANK");
    case OP_ISA_WORDCHAR              :return("ISA WORDCHAR");
    case OP_ISA_INT                   :return("ISA INT");
    case OP_ISA_HEXINT                :return("ISA HEXINT");
    case OP_ISA_FLOAT                 :return("ISA FLOAT");
    case OP_ISA_BLANKSEQ              :return("ISA BLANKSEQ");
    case OP_ISA_WORD                  :return("ISA WORD");
    case OP_ISA_SIGN                  :return("ISA SIGN");
    case OP_ISA_BALANCED              :return("ISA BALANCED");
    case OP_ISA_QSTRING               :return("ISA QSTRING");
    case OP_ISN | OP_ISA_ANY          :return("ISN ANY");
    case OP_ISN | OP_ISA_NOTEOL       :return("ISN NOTEOL");
    case OP_ISN | OP_ISA_EOL          :return("ISN EOL");
    case OP_ISN | OP_ISA_UPPER        :return("ISN UPPER");
    case OP_ISN | OP_ISA_LOWER        :return("ISN LOWER");
    case OP_ISN | OP_ISA_ALPHA        :return("ISN ALPHA");
    case OP_ISN | OP_ISA_ALNUM        :return("ISN ALNUM");
    case OP_ISN | OP_ISA_PUNCT        :return("ISN PUNCT");
    case OP_ISN | OP_ISA_CTRL         :return("ISN CTRL");
    case OP_ISN | OP_ISA_DIGIT        :return("ISN DIGIT");
    case OP_ISN | OP_ISA_HEXDIGIT     :return("ISN HEXDIGIT");
    case OP_ISN | OP_ISA_SPACE        :return("ISN SPACE");
    case OP_ISN | OP_ISA_BLANK        :return("ISN BLANK");
    case OP_ISN | OP_ISA_WORDCHAR     :return("ISN WORDCHAR");
    case OP_ISN | OP_ISA_INT          :return("ISN INT");
    case OP_ISN | OP_ISA_HEXINT       :return("ISN HEXINT");
    case OP_ISN | OP_ISA_FLOAT        :return("ISN FLOAT");
    case OP_ISN | OP_ISA_BLANKSEQ     :return("ISN BLANKSEQ");
    case OP_ISN | OP_ISA_WORD         :return("ISN WORD");
    case OP_ISN | OP_ISA_SIGN         :return("ISN SIGN");
    case OP_ISN | OP_ISA_BALANCED     :return("ISN BALANCED");
    case OP_ISN | OP_ISA_QSTRING      :return("ISN QSTRING");
    case OP_BGN                       :return("BGN");
    case OP_END                       :return("END");
    case OP_FOR_PLUS                  :return("FOR 1,*");
    case OP_FOR_STAR                  :return("FOR 0,*");
    case OP_STO                       :return("STO");
    case OP_RCL                       :return("RCL");
    case OP_RST                       :return("RST");
    case OP_ONF                       :return("ONF");
    case OP_JPM                       :return("JPM");
    case OP_JPF                       :return("JPF");
    case OP_NXT                       :return("NXT");
    case OP_FOR                       :return("FOR");
    case OP_CHR                       :
    case OP_CHR_U                     :return("CHR");
    case OP_RNG                       :
    case OP_RNG_U                     :return("RNG");
    case OP_CHN                       :
    case OP_CHN_U                     :return("CHN");
    case OP_RGN                       :
    case OP_RGN_U                     :return("RGN");
    case OP_CHR_FOLD                  :
    case OP_CHR_U_FOLD                :return("CHR FOLD");
    case OP_RNG_FOLD                  :
    case OP_RNG_U_FOLD                :return("RNG FOLD");
    case OP_CHN_FOLD                  :
    case OP_CHN_U_FOLD                :return("CHN FOLD");
    case OP_RGN_FOLD                  :
    case OP_RGN_U_FOLD                :return("RGN FOLD");
    case OP_CHH3                      :
    case OP_CHH4                      :return("CHH");
    case OP_HDR_NCAPT                 :return("HDR LVL");
    case OP_HDR_START                 :return("HDR LEN");
    case OP_HDR_END                   :return("HDR END");
    default                           :return ("???");
  }
}

static uint16_t onf[16];
static uint16_t cur_lvl=0;

static uint8_t *print_line(uint8_t *p, uint16_t cnt)
{
  int      k = 0;
  uint8_t *args = p+1;
  uint8_t  op = *p;
  char    *opstr;
  
  opstr=opcode_str(op);
  if (*opstr == '?') opstr = opcode_str(op & 0xF0);

  switch (op & 0xF0) {
    case OP_ONF:  onf[cur_lvl] = ((p[0] & 0x0F) << 8) | p[1];
                  break;
    case OP_BGN:  cur_lvl++;
                  break;
    case OP_END:  cur_lvl--;
                  break;
  }

  printf("%03X  ",cnt);

  while (k++ < skpoplen(op)) printf("%02X ", *p++);
  while (k++ < 4)            printf("   ");

  printf(" %s ",opstr);

       if (op == 0xFF)  { printf("%X%02X ",(args[0] & 0x0F),args[1]); }
  else if (op >= 0xF0)  { printf("%02X%02X  ",args[0], args[1]); }
  else if (op >= 0xE0)  { printf("%02X    ",args[0]); }
  else if (op >= 0xD0)  { }
  else if (op >= 0x90)  { printf("%X%02X   ",(op & 0x0F),args[0]); }
  else if (op >= 0x60)  { }
  else if (op >= 0x40)  { printf("%02X ",(op & 0x0F)); }
#if 0
  if (op != 0xFF && op != 0xEF) {
    switch (op & 0xF0) {
      case OP_NXT:
      case OP_CHR_BASE:
      case OP_CHR_U_BASE:
                    printf("  ;JPF %03X",onf[cur_lvl]);
      default:      break;
    }
  }
#endif
  printf("\n");
  return p;
}

void skpdisasm(char *prg, FILE *f)
{
  uint8_t *p = (uint8_t *)prg;
  uint8_t *x;

  if (!prg) return; 
  if (f == NULL) f = stdout;
  
  while (*p) {
    x=p;
    p = print_line(p, p-(uint8_t *)prg);
    if (x==p) break;
  }
  print_line(p, p-(uint8_t *)prg);
}
