#include "./headers/acquireImage.h"

/********************************************************************************************/
/*  Function:                                                                               */
/*    - 'acquireImage'                                                                      */
/*  Inputs:                                                                                 */
/*    - fptr:      File pointer to current HI-1A image FITS file                            */
/*  Output:                                                                                 */
/*    - image:      Pointer to created 1-D HI-1A image array                                */
/********************************************************************************************/

void acquireImage(fitsfile *fptr, float *image)
{

/************************************************************/
/* Reads HI-1A image data from a FITS file image extension  */
/* and writes the data into a 1-D array.                    */
/************************************************************/

/*
  IMAGE variable initialization */
  int status = 0;   /* CFITSIO status value MUST be initialized to zero! */
  int naxis = 0;    /* Initialise variable holding number of image axes */
  int bitpix, bytepix, datatype = 0, anynul;  /* CFITSIO variables */
  int NaNcount = 0, NEGcount = 0; /* Counts how many NaN and negative flux pixels exist in image */
  long naxes[2] = {1, 1};         /* Initialise array holding image axes dimensions in pixels */
  long first = 1, npix = 1;       /* CFITSIO variables */
  float *array;   /* Image reshaped into 1D format */
  float arrval;   /* Value of image pixel */
  double nulval = 0.0;

  printf("acquiring image.\n");

/*******************
  Get image parameters from fits header   */
  fits_get_img_param(fptr, 2, &bitpix, &naxis, naxes, &status);

/* Determine total number of image pixels to read along each axis */
  for (int ia = 0; ia < 2; ia++)
  {
    npix *= naxes[ia];
    if (naxes[ia] != N_AXES) {
      printf("Axis size received: %ld,  expected: %d\n", naxes[ia], N_AXES);
      fits_close_file(fptr, &status);
      exit(-1);
    }
  }

  bytepix = abs(bitpix) / 8;  /* Calculate how many bytes for each pixel value */
  datatype = TFLOAT;          /* Type of data of each pixel value */

/* Allocate memory to read 2D image into 1D array */
  array = (float *) calloc(npix, bytepix);

/*******************
  Read image table data into 1D array */
  fits_read_img(fptr, datatype, first, npix, &nulval, array, &anynul, &status);

/*******************
Convert 1D array into 2D image array */
  for (int jj = 0; jj < N_AXES; jj++){
    for (int ii = 0; ii < N_AXES; ii++){
      arrval = array[(jj*N_AXES)+ii];
      if(isnan(arrval)){
        NaNcount++;
        *(image + ii*N_AXES + jj) = 1e-17;
      }
      else if(arrval < 0){
        NEGcount++;
        *(image + ii*N_AXES + jj) = 1e-17;
      }
      else
        *(image + ii*N_AXES + jj) = arrval;
    }
  }
  free(array);

  if (status) fits_report_error(stderr, status); /* print any error message */

}
