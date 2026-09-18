#include "sqrt_16.h"

unsigned short int sqrt_16(unsigned int M) 
{ 
     unsigned short int N, i; 
     unsigned int tmp, ttp;    // ??????? 
     if (M == 0)                // ????,??????0 
         return 0; 

     N = 0; 

     tmp = (M >> 30);           // ?????:B[m-1] 
     M <<= 2; 
     if (tmp > 1)               // ????1 
     { 
         N ++;                  // ??????1,??????0 
         tmp -= N; 
     } 

     for (i=15; i>0; i--)       // ????15? 
     { 
         N <<= 1;               // ???? 

         tmp <<= 2; 
         tmp += (M >> 30);      // ?? 

         ttp = N; 
         ttp = (ttp<<1)+1; 

         M <<= 2; 
         if (tmp >= ttp)        // ???? 
         { 
             tmp -= ttp; 
             N ++; 
         } 

     } 

     return N; 
}
