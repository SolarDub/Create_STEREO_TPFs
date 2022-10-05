#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

/*==========================================================================*/

int getStarCoords(int Ns, char *filename, char *star, double *coords)
{

/***************************************************************************/
/* Constructs single array from RA & DEC arrays                             */
/***************************************************************************/

/*
  File pointer variable declaration */

  FILE *fptr;

  int status;

  if ((fptr = fopen(filename,"r")) == NULL)
  {
    printf("Cannot open file %s.\nExiting.\n",filename);
    status = -1;
    return status;
  }
  else
  {
    for (int is = 0; is < Ns+1; is++){
      status = fscanf(fptr,"%s %lf %lf %lf %lf %lf %lf"
          ,star, &coords[0], &coords[1], &coords[2], &coords[3], &coords[4], &coords[5]);
    }
  }

  fclose(fptr);

  printf("\nStar:\t%s.\nRA:\t%2.0fh %2.0fm %6.3fs\nDEC:\t%2.0fd %2.0fm %6.3fs\n\n"\
        ,star, coords[0],  coords[1],  coords[2]\
             , coords[3],  coords[4], coords[5]);

  return status;
}
