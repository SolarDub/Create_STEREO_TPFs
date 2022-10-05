#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "fitsio.h"

#define N_AXES 1024

int main(){

/***************************************************************************/
/* Self-contained program to read in an image from a hard-coded, selected  */
/* .fits file for a hard-coded star. Flux values for a pixel can be found. */
/***************************************************************************/

/*
  FILE variable initialization */
  int year, month, day, hr, min, sec;
  char s_day[3], s_month[3], s_year[5], s_date[12];
  char s_hr[3], s_min[3], s_sec[3], s_time[10];
  char dir_name[80], file_name[40];

/*
  IMAGE variable initialization */
  fitsfile *fptr;         /* FITS file pointer, defined in fitsio.h */
  int status = 0;   /* CFITSIO status value MUST be initialized to zero! */
  int ii, jj, naxis = 0;
  int bitpix, bytepix, datatype = 0, anynul;
  long naxes[2] = {1, 1};
  long first, npix = 1;
  float *array, image[N_AXES][N_AXES];
  double nulval = 0.0;

/*
  STAR variable initialization */
  float RAhms[3];
  float DEChms[3];
  char star[30];

  printf("\n");

/*
  Select STAR */
  strcpy(star, "23Tau");

/*
  STAR celestial coordinates */
  RAhms[0] = 3;       DEChms[0] =  23;
  RAhms[1] = 46;      DEChms[1] =  56;
  RAhms[2] = 19.574;  DEChms[2] =  54.080;

  printf("Star: %s. RA: %2.0fh %2.0fm %6.3fs DEC: %2.0fd %2.0fm %6.3fs\n"\
        ,star, RAhms[0],  RAhms[1],  RAhms[2]\
             , DEChms[0], DEChms[1], DEChms[2]);


/*******************
  FILE date & time */
  year  = 2008;     hr  = 0; 
  month = 4;        min = 9;
  day   = 2;        sec = 1;

  sprintf(s_year, "%d", year);
  sprintf(s_month, "%02d", month);
  sprintf(s_day, "%02d", day);
  sprintf(s_date, "%s%s%s",s_year,s_month,s_day);

  sprintf(s_hr, "%02d", hr);
  sprintf(s_min, "%02d", min);
  sprintf(s_sec, "%02d", sec);
  sprintf(s_time, "%s%s%s",s_hr,s_min,s_sec);

  sprintf(file_name,"%s_%s_24h1A_br01.fits",s_date,s_time);
  printf("Input file: %s\n",file_name);
/******************/


/*******************
  Test then open fits file   */
  if (!fits_open_file(&fptr, file_name, READONLY, &status))
  {
  printf("passed");

    for (ii = 0; ii < 2; ii++)
      naxes[ii] = 1;

/*******************
  Get image parameters from fits header   */
    fits_get_img_param(fptr, 2, &bitpix, &naxis, naxes, &status);

/* Calculate total number of image pixels to read */
    for (ii = 0; ii < 2; ii++)
    {
      npix *= naxes[ii];
      if (naxes[ii] != N_AXES) {
        printf("Axis size received: %ld,  expected: %d\n", naxes[ii], N_AXES);
        fits_close_file(fptr, &status);
        return 0;
      }
    }

    bytepix = abs(bitpix) / 8;  /* Calculate how many bytes for each pixel value */
    datatype = TFLOAT;          /* Type of data of each pixel value */

/* Allocate memory to read in 2D image into 1D array */
    array = (float *) calloc(npix, bytepix); 

/*******************
  Read image table data into 1D array */
    first = 1;
    fits_read_img(fptr, datatype, first, npix, 
                  &nulval, array, &anynul, &status);

/*******************
  Convert 1D array into 2D image array */
    for (jj = 0; jj < N_AXES; jj++)
      for (ii = 0; ii < N_AXES; ii++)
        image[ii][jj] = array[(jj*1024)+ii];

/*******************
  Close fits file */
   fits_close_file(fptr, &status);
  }

  printf("\n");

  if (status) fits_report_error(stderr, status); /* print any error message */
  return (status);
}