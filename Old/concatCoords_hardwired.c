#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*==========================================================================*/

void concatCoords(char *star, double *RAhms, double *DEChms, double *coords){

/***************************************************************************/
/* Constructs single array from RA & DEC arrays                             */
/***************************************************************************/

/*
  LOOP variable initialization */
  int lp;

  /* Concatenate for passing into functions */
  for(lp = 0; lp < 3; lp++)
  {
    coords[lp]   = RAhms[lp];
    coords[lp+3] = DEChms[lp];
  }

  printf("\nStar:\t%s.\nRA:\t%2.0fh %2.0fm %6.3fs\nDEC:\t%2.0fd %2.0fm %6.3fs\n"\
        ,star, RAhms[0],  RAhms[1],  RAhms[2]\
             , DEChms[0], DEChms[1], DEChms[2]);
}

