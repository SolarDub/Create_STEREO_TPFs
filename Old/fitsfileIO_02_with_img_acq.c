#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "fitsio.h"
#include "./include/coords2pixels.h"

#define PI 3.141592654
#define N_AXES 1024
#define N_STAMP 10


int main(){

/***************************************************************************/
/* Self-contained program to read in an image from a hard-coded, selected  */
/* .fits file for a hard-coded star. Flux values for a pixel can be found. */
/***************************************************************************/

/*
  STAR variable initialization */
  float RAhms[3], DEChms[3], coords[6];
  char star[30];
  int px=0, py=0;

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
  int NaNcount = 0, NEGcount = 0;
  long naxes[2] = {1, 1};
  long first, npix = 1;
  float *array, image[N_AXES][N_AXES], arrval;
  double nulval = 0.0;

/*
  STAMP variable initialization */
  float stamp[N_STAMP][N_STAMP];

/*
  OTHER variable initialization (for loops, etc.) */
  int lp;

  printf("\n");

/*
  Select STAR */
  strcpy(star, "23Tau");

/*
  STAR celestial coordinates */
  RAhms[0] = 3;       DEChms[0] =  23;
  RAhms[1] = 46;      DEChms[1] =  56;
  RAhms[2] = 19.574;  DEChms[2] =  54.080;

  /* Concatenate for passing into functions */
  for(lp = 0; lp < 3; lp++)
  {
    coords[lp]   = RAhms[lp];
    coords[lp+3] = DEChms[lp];
  }

  printf("Star: %s.\nRA: %2.0fh %2.0fm %6.3fs\nDEC: %2.0fd %2.0fm %6.3fs\n"\
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
  printf("\nInput file: %s\n",file_name);
/******************/


/*******************
  Test then open fits file   */
  if (!fits_open_file(&fptr, file_name, READONLY, &status))
  {

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
    for (jj = 0; jj < N_AXES; jj++){
      for (ii = 0; ii < N_AXES; ii++){
        arrval = array[(jj*1024)+ii];
        if(isnan(arrval)){
          NaNcount++;
          image[ii][jj] = 1e-17;
        }
        else if(arrval < 0){
          NEGcount++;
          image[ii][jj] = 1e-17;
        }
        else
          image[ii][jj] = arrval;
      }
    }
   printf("\nNaN count: %d\n",NaNcount);
   printf("NEG count: %d out of %d pixels\n",NEGcount, N_AXES*N_AXES);
   
   coords2pixels(fptr, coords, &px,&py);

   printf("\npx: %d\tpy %d\n", px, py);

/*******************
  Close fits file */
    fits_close_file(fptr, &status);
  }

  for(jj=0; jj<N_STAMP; jj++){
    for(ii=0; ii<N_STAMP; ii++){
      stamp[ii][jj]=image[px-(N_STAMP/2)+ii][py-(N_STAMP/2)+jj];
    }
  }
  
  printf("\n(0,0):\t%f\n(9,9):\t%f\n",stamp[0][0],stamp[9][9]);

  printf("\n");

  if (status) fits_report_error(stderr, status); /* print any error message */
  return (status);
}
