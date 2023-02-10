#include "./headers/produceStamp.h"

/********************************************************************************************/
/*  Function:                                                                               */
/*    - 'produceStamp'                                                                      */
/*  Inputs:                                                                                 */
/*    - image:      File pointer to current HI-1A image FITS file                           */
/*    - px, py:     Pixel coordinates of star in HI-1A image                                */
/*  Output:                                                                                 */
/*    - stamp:      Target image stamp                                                      */
/********************************************************************************************/

void produceStamp(float *image, int px, int py, float *stamp)
{

/***************************************************************************/
/* Constructs image stamp for target star from base image                   */
/***************************************************************************/

  for(int jj=0; jj<N_STAMP; jj++){
    for(int ii=0; ii<N_STAMP; ii++){

      *(stamp + ii*N_STAMP + jj) = *(image + ((px-(N_STAMP/2)+ii)*N_AXES) + py-(N_STAMP/2)+jj);

    }
  }

  printf("\nTarget star image stamp produced.\n");

}
