#ifndef __FONT_H
#define __FONT_H 	   

extern const unsigned char asc2_1206[95][12];
extern const unsigned char asc2_1608[95][16];
extern const unsigned char asc2_2412[95][36];
typedef struct 
{
       unsigned char Index[2];	
       char Msk[32];
}typFNT_GB16; 
extern typFNT_GB16 tfont16[];

typedef struct 
{
      unsigned char Index[2];	
      char Msk[72];
}typFNT_GB24;
extern typFNT_GB24 tfont24[];

typedef struct 
{
      unsigned char Index[2];	
      char Msk[128];
}typFNT_GB32; 
extern typFNT_GB32 tfont32[];

#endif

