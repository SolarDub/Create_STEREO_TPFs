#include "./headers/distArgs.h"

/***********************************************************************************************/
/*  Function:                                                                                  */
/*    - 'distArgs'                                                                             */
/*  Inputs:                                                                                    */
/*    - argc:            Number of arguments passed to main executable                         */
/*    - argv:            Array of string arguments passed to main executable                   */
/*  Output:                                                                                    */
/*    - ptr_star:        Pointer to star properties structure                                  */
/*      - star:            Star name                                                           */
/*      - RAhms:           Right Ascension in hours, minutes, seconds                          */
/*      - DECdms:          Declination in degrees, minutes, seconds                            */
/*    - ptr_dir:         Pointer to directory paths structure                                  */
/*      - base:            Directory of executable, used for creating filepath to              */
/*                         orbit start/end time lookup table                                   */
/*      - HI1A_data:       Pointer to string filepath to base STEREO HI-1A data directory      */
/*      - stereo_star:     Pointer to string filepath to base stellar data directory           */
/*    - orbits:          Array of orbit numbers to be analysed                                 */
/*    - Norbs:           Number of orbits to be analysed                                       */
/***********************************************************************************************/

void distArgs(int argc, char *argv[], TARG *ptr_star, PTH *ptr_dir, int *orbits, int Norbs)
{

  /* Load star name from input arguments into variable */
  ptr_star->name = argv[1];

  /* Load star celestial (RA, Dec) coordinates from input arguments into respective arrays */
  for (int ic = 0; ic < 3; ic++){
    ptr_star->RAhms[ic]  = strtod(argv[ic+2],NULL);  /* h m s */
    ptr_star->DECdms[ic] = strtod(argv[ic+5],NULL);  /* d m s */
  }

  /* Load directory paths from input arguments into variables */
  ptr_dir->base        = argv[8];    /* Base directory (includes this program, etc) */
  ptr_dir->HI1A_data   = argv[9];    /* STEREO HI-1A base image fits file directory */
  ptr_dir->stereo_star = argv[10];   /* TPF fits file output directory */

  /* Load orbit numbers from arguments into orbits array */
  for (int ioa = 0; ioa < Norbs; ioa++){
    orbits[ioa] = strtod(argv[ioa+(argc - Norbs)],NULL);   /* first_orbit_arg = argc - Norbs (see main listing) */
  }

}
