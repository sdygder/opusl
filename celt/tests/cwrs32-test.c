#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>

#ifndef CUSTOM_MODES
#define CUSTOM_MODES
#else
#define TEST_CUSTOM_MODES
#endif

#define CELT_C
#include "../celt/stack_alloc.h"
#include "../celt/entenc.c"
#include "../celt/entdec.c"
#include "../celt/entcode.c"
#include "../celt/cwrs.c"
#include "../celt/mathops.c"
#include "../celt/rate.h"

#define NMAX (240)
#define KMAX (128)

#ifdef TEST_CUSTOM_MODES

#define NDIMS (46)
static const int pn[NDIMS]={
   2,   3,   4,   5,   6,   7,   8,   9,  10,
  11,  12,  13,  14,  15,  16,  18,  20,  22,
  24,  26,  28,  30,  32,  36,  40,  44,  48,
  52,  56,  60,  64,  72,  80,  88,  96, 104,
 112, 120, 128, 144, 160, 176, 192, 208, 224,
 240
};
static const int pkmax[NDIMS]={
 128, 128, 128, 128,  88,  52,  36,  26,  22,
  18,  16,  15,  13,  12,  12,  11,  10,   9,
   9,   8,   8,   7,   7,   7,   7,   6,   6,
   6,   6,   6,   5,   5,   5,   5,   5,   5,
   4,   4,   4,   4,   4,   4,   4,   4,   4,
   4
};

#else /* TEST_CUSTOM_MODES */

#define NDIMS (22)
static const int pn[NDIMS]={
   2,   3,   4,   6,   8,   9,  11,  12,  16,
  18,  22,  24,  32,  36,  44,  48,  64,  72,
  88,  96, 144, 176
};
static const int pkmax[NDIMS]={
 128, 128, 128,  88,  36,  26,  18,  16,  12,
  11,   9,   9,   7,   7,   6,   6,   5,   5,
   5,   5,   4,   4
};

#endif

int main(void){
  int t;
  int n;
  ALLOC_STACK;
  for(t=0;t<NDIMS;t++){
    int pseudo;
    n=pn[t];
    for(pseudo=1;pseudo<41;pseudo++)
    {
      int k;
      opus_uint32 uu[KMAX+2U];
      opus_uint32 inc;
      opus_uint32 nc;
      opus_uint32 i;
      k=get_pulses(pseudo);
      if (k>pkmax[t])break;
      printf("Testing CWRS with N=%i, K=%i...\n",n,k);
      nc=ncwrs_urow(n,k,uu);
      inc=nc/20000;
      if(inc<1)inc=1;
      for(i=0;i<nc;i+=inc){
        opus_uint32 u[KMAX+2U];
        int           y[NMAX];
        int           sy;
        int           yy[5];
        opus_uint32 v;
        opus_uint32 ii;
        int           kk;
        int           j;
        memcpy(u,uu,(k+2U)*sizeof(*u));
        cwrsi(n,k,i,y,u);
        sy=0;
        for(j=0;j<n;j++)sy+=ABS(y[j]);
        if(sy!=k){
          fprintf(stderr,"N=%d Pulse count mismatch in cwrsi (%d!=%d).\n",
           n,sy,k);
          return 99;
        }
        /*printf("%6u of %u:",i,nc);
        for(j=0;j<n;j++)printf(" %+3i",y[j]);
        printf(" ->");*/
        ii=icwrs(n,k,&v,y,u);
        if(ii!=i){
          fprintf(stderr,"Combination-index mismatch (%lu!=%lu).\n",
           (long)ii,(long)i);
          return 1;
        }
        if(v!=nc){
          fprintf(stderr,"Combination count mismatch (%lu!=%lu).\n",
           (long)v,(long)nc);
          return 2;
        }
#ifndef SMALL_FOOTPRINT
        if(n==2){
          cwrsi2(k,i,yy);
          for(j=0;j<2;j++)if(yy[j]!=y[j]){
            fprintf(stderr,"N=2 pulse vector mismatch ({%i,%i}!={%i,%i}).\n",
             yy[0],yy[1],y[0],y[1]);
            return 3;
          }
          ii=icwrs2(yy,&kk);
          if(ii!=i){
            fprintf(stderr,"N=2 combination-index mismatch (%lu!=%lu).\n",
             (long)ii,(long)i);
            return 4;
          }
          if(kk!=k){
            fprintf(stderr,"N=2 pulse count mismatch (%i,%i).\n",kk,k);
            return 5;
          }
          v=ncwrs2(k);
          if(v!=nc){
            fprintf(stderr,"N=2 combination count mismatch (%lu,%lu).\n",
             (long)v,(long)nc);
            return 6;
          }
        }
        else if(n==3){
          cwrsi3(k,i,yy);
          for(j=0;j<3;j++)if(yy[j]!=y[j]){
            fprintf(stderr,"N=3 pulse vector mismatch "
             "({%i,%i,%i}!={%i,%i,%i}).\n",yy[0],yy[1],yy[2],y[0],y[1],y[2]);
            return 7;
          }
          ii=icwrs3(yy,&kk);
          if(ii!=i){
            fprintf(stderr,"N=3 combination-index mismatch (%lu!=%lu).\n",
             (long)ii,(long)i);
            return 8;
          }
          if(kk!=k){
            fprintf(stderr,"N=3 pulse count mismatch (%i!=%i).\n",kk,k);
            return 9;
          }
          v=ncwrs3(k);
          if(v!=nc){
            fprintf(stderr,"N=3 combination count mismatch (%lu!=%lu).\n",
             (long)v,(long)nc);
            return 10;
          }
        }
        else if(n==4){
          cwrsi4(k,i,yy);
          for(j=0;j<4;j++)if(yy[j]!=y[j]){
            fprintf(stderr,"N=4 pulse vector mismatch "
             "({%i,%i,%i,%i}!={%i,%i,%i,%i}.\n",
             yy[0],yy[1],yy[2],yy[3],y[0],y[1],y[2],y[3]);
            return 11;
          }
          ii=icwrs4(yy,&kk);
          if(ii!=i){
            fprintf(stderr,"N=4 combination-index mismatch (%lu!=%lu).\n",
             (long)ii,(long)i);
            return 12;
          }
          if(kk!=k){
            fprintf(stderr,"N=4 pulse count mismatch (%i!=%i).\n",kk,k);
            return 13;
          }
          v=ncwrs4(k);
          if(v!=nc){
            fprintf(stderr,"N=4 combination count mismatch (%lu!=%lu).\n",
             (long)v,(long)nc);
            return 14;
          }
        }
#endif /* SMALL_FOOTPRINT */
        /*printf(" %6u\n",i);*/
      }
      /*printf("\n");*/
    }
  }
  return 0;
}