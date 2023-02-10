#include "./headers/coordTransforms.h"

/********************************************************************************************/
/*  Function:                                                                               */
/*    - 'RAhms2deg'                                                                         */
/*  Inputs:                                                                                 */
/*    - RAhms:     Pointer to 3-element array holding Right Ascension in h, m, s            */
/*  Output:                                                                                 */
/*    -            Right Ascension in degrees                                               */
/********************************************************************************************/

double RAhms2deg(double *RAhms)
{

  return(15 * (RAhms[0] + (RAhms[1] + (RAhms[2]/60.0))/60.0));

}

/********************************************************************************************/
/*  Function:                                                                               */
/*    - 'DECdms2deg'                                                                        */
/*  Inputs:                                                                                 */
/*    - DEChms:     Pointer to 3-element array holding Declination in d, m, s               */
/*  Output:                                                                                 */
/*    -            Declination in degrees                                                   */
/********************************************************************************************/

double DECdms2deg(double *DECdms)
{

  int sign = 1;   /* N (1) or S (-1) of celestial equator used for calculating dec in degrees from dms */

  /* Determine whether star is in N or S celestial hemisphere */
  if ((DECdms[0] == 0 && DECdms[1] == 0 && DECdms[2] < 0) ||
      (DECdms[0] == 0 && DECdms[1] < 0 ) ||
      (DECdms[0] < 0 ))
     sign = -1;

  return(sign * (fabs(DECdms[0]) + (fabs(DECdms[1]) + (fabs(DECdms[2])/60.0))/60.0));

}
