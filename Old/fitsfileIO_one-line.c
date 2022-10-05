#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <dirent.h>
#include "fitsio.h"

#define PI 3.141592654
#define N_AXES 1024
#define N_STAMP 20

void getStarCoords(char *filename, char *star, double *coords);
void createFilename(int *date_ymd, int *time_hms, char *file_name);
void image_date_time(int *date_ymd, int *time_hms);
void acquireImage(fitsfile *, char *, float *image, int);
void coords2pixels(fitsfile *, double *coords, int *, int *);
double STEREO_UTC2BJD(int *date_ymd, int *time_hms, int, int);
int regionCheck(char *star, int, int, int, int);
void produceStamp(float *image, int, int, float *stamp, int, int);
void output2fits(char *filename, int , double *BJD, int , float *s, int , int *p1, int *p2);


int main(){

/****************************************************************************/
/* Main program to read in an image from a hard-coded, selected .fits file  */
/* for a hard-coded star. Flux values for a pixel can be found.             */
/****************************************************************************/

/*
  STAR variable initialization */
  double coords[6];
  char star[30];
  char coorfilename[] = "starCoords.txt";

/*
  INPUT FILE arrays initialization */
  int date_ymd[3], time_hms[3];
  char file_name[100];

/*
  TIME variable initialization */
  int t_str = 541;                  /* Time of first image of the day in seconds hhmmss: 000901 (not 2007) */
  int dt = 40*60;                   /* Time between images in seconds (40 minute cadence) */
  int ipd = 36;                     /* Number of images in a day */
  int t_end = t_str + dt*(ipd-1);   /* Time of first image of the day in seconds */
  int it;

/*
  FITSFILE variable initialization */
  fitsfile *infptr, *outptr;         /* FITS file pointers, defined in fitsio.h */
  int status = 0;                    /* CFITSIO status value MUST be initialized to zero! */

/*
  IMAGE array initialization */
  float image[N_AXES][N_AXES];
  float *ptr_image;

/*
  STAMP array initialization */
  int px=0, py=0;
  float stamp[N_STAMP][N_STAMP];
  float *ptr_stamp;

/*
  OTHER variable initialization (for loops, etc.) */
  int ii, jj, rr, tt, regchk, miss, N_good_img, TL = 2*ipd; // TL = 20*ipd;
  char str1[8], str2[8], str3[8], str4[8], str5[8], str6[8];
  double flt1, flt2, flt3, flt4, flt5, flt6;

/*
  SURVEY arrays initialization */
  unsigned short t[TL];           /* time */
  int p1[TL], p2[TL];             /* x- & y-pixels */
  float s[TL][N_STAMP][N_STAMP];  /* target imagestamps - Note reversal of dimensions */
  double BJD[TL];                 /* BJD time */
  double offset = 2450000.0;      /* BJD offset */

/*
  OUTPUT FITS FILE variable initialization */
  char outputfilename[100];


/***************************************************************************/

/*********************************************
  Input star name and its celestial coordinates from file */

  getStarCoords(coorfilename, star, coords);

/*************************
  FILE Start date & time */

  date_ymd[0]  = 2008;     time_hms[0]  = 0; 
  date_ymd[1]  = 4;        time_hms[1]  = 9;
  date_ymd[2]  = 1;        time_hms[2]  = 1;

  for (it = 0; it < TL; it++){

    printf("\n***************************************************************************\n");

    if (it > 0)
      image_date_time(date_ymd, time_hms);

    t[it] = it;

    createFilename(date_ymd, time_hms, file_name);

/*******************************
    Test then open fits file   */

    if (!fits_open_file(&infptr, file_name, READONLY, &status)){

/******************************************
      Acquire full image from fits file   */

      ptr_image = &image[0][0];
      acquireImage(infptr, file_name, ptr_image, N_AXES);

/***************************************************************************
      Find star's image pixel coordinates from its celestial coordinates   */

      coords2pixels(infptr, coords, &px, &py);
      p1[it] = px; p2[it] = py;
      BJD[it]=STEREO_UTC2BJD(date_ymd, time_hms, px, py) - offset;

/***************************************************************************
      Check whether star is enough within image to produce target stamp */

      regchk = regionCheck(star, px, py, N_AXES, N_STAMP);

      if (regchk == 1)         /* Star not yet entered image domain */
      {
        BJD[it]=0.0;    /* Sets a flag to ignore/remove bad data */
        miss += 1;
        continue;       /* Jump to next data acquisition loop */
      }
      else if (regchk == 2)    /* Star has left image domain */
      {                
        BJD[it]=0.0;    /* Sets a flag to ignore/remove bad data */
        fits_close_file(infptr, &status);  /* No more data to collect - close fits file */
        miss += 1;
        break;          /* Leave data acquisition loop */         
      }

/************************
      Close fits file   */
      fits_close_file(infptr, &status);     /*  Close fits file */
    }
    else{
      if (status) fits_report_error(stderr, status);    /* print any error message */
      printf("\n\n***Missing fits file***\n%s\n\n",file_name);
      miss += 1;
      status = 0;   /* Reset status to zero */
      continue;
    }               /* End of if statement opening input image fits file */

/**********************************
    Produce target image stamp   */  

    ptr_stamp = &stamp[0][0];

    produceStamp(ptr_image, px, py, ptr_stamp, N_AXES, N_STAMP);
    for(jj = 0; jj < N_STAMP; jj++){
      for(ii = 0; ii < N_STAMP; ii++){
        s[it][jj][ii] = stamp[ii][jj];    /* Note reversal of s dimensions */
      }  
    }
    printf("\nSaved target image to array\n");

    N_good_img += 1;
    printf("\n");
  }                  /* End of for loop processing a single image */

  printf("\nMissing files:%d\n",miss);
  printf("Good images:%d\n",N_good_img);
  printf("\n");
  printf("\n");

/******************************
  Produce output fitsfile   */  

  /* Create filename (! means to overwrite existing file) */
  sprintf(outputfilename, "!writeTestBinaryTable_%s.fits", star);

  output2fits(outputfilename, TL, BJD, N_STAMP, &s[0][0][0], N_good_img, p1, p2);

  return 0;

} /* End of program */
