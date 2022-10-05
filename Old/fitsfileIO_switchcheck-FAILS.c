#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <dirent.h>
#include "fitsio.h"
#include "./include/STEREO_UTC2BJD.h"
#include "./include/coords2pixels.h"
#include "./include/acquireImage.h"
#include "./include/miniFuncs.h"

#define PI 3.141592654
#define N_AXES 1024
#define N_STAMP 10

void concatCoords(char *star, float RAhms[], float DEChms[], float coords[]);
void createFilename(int date_ymd[], int time_hms[], char *file_name);
void image_date_time(int date_ymd[], int time_hms[]);
void acquireImage(fitsfile *, char *, float image[][N_AXES]);
void coords2pixels(fitsfile *, float coords[], int *, int *);
double STEREO_UTC2BJD(int date_ymd[], int time_hms[], int, int);
int regionCheck(char *star, int, int);
void produceStamp(float image[][N_AXES], int, int, float stamp[][N_STAMP]);

int main(){

/***************************************************************************/
/* Self-contained program to read in an image from a hard-coded, selected  */
/* .fits file for a hard-coded star. Flux values for a pixel can be found. */
/***************************************************************************/

/*
  STAR variable initialization */
  float RAhms[3], DEChms[3], coords[6];
  char star[30];

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

/*
  STAMP array initialization */
  int px=0, py=0;
  float stamp[N_STAMP][N_STAMP];

/*
  OTHER variable initialization (for loops, etc.) */
  int ii, jj, rr, regchk, TL = 3*ipd;

/*
  SURVEY arrays initialization */
  unsigned short t[TL];           /* time */
  int p1[TL], p2[TL];             /* x- & y-pixels */
  float s[TL][N_STAMP][N_STAMP];  /* target imagestamps - Note reversal of dimensions */
  double BJD[TL];                 /* BJD time */
  double offset = 2450000.0;      /* BJD offset */

/*
  OUTPUT FITS FILE variable initialization */
  char outputfilename[80];
  int hdunum, hdutype, colnum, tfields;
  long nrows, firstrow, firstelem, nelements;

/*
  OUTPUT FITS FILE HEADER variable initialization */
  short n_cols = 4;
  char *ttype[] = { "TIME", "IMAGE", "X_PIXEL", "Y_PIXEL" };  char *tforma[] = { "F12.7", "F10.6", "I6", "I6"}; /* special CFITSIO codes */  char *tformb[] = { "D", "100E", "1V", "1V"}; /* special CFITSIO codes */  char *tunit[] = { "BJD - 2454833", "DN/s/CCDPIX", "pixel", "pixel" };  
  char extname[] = "TARGETTABLES"; /* extension name */



/***************************************************************************/

/**************
  Select STAR */

  strcpy(star, "23Tau");

/*****************************
  STAR celestial coordinates */

  RAhms[0] = 3;       DEChms[0] =  23;
  RAhms[1] = 46;      DEChms[1] =  56;
  RAhms[2] = 19.574;  DEChms[2] =  54.080;

  concatCoords(star, RAhms, DEChms, coords);

/*************************
  FILE Start date & time */

  date_ymd[0]  = 2008;     time_hms[0]  = 0; 
  date_ymd[1]  = 4;        time_hms[1]  = 9;
  date_ymd[2]  = 18;        time_hms[2]  = 1;

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

      acquireImage(infptr, file_name, image);

/***************************************************************************
      Find star's image pixel coordinates from its celestial coordinates   */

      coords2pixels(infptr, coords, &px, &py);
      p1[it] = px; p2[it] = py;
      BJD[it]=STEREO_UTC2BJD(date_ymd, time_hms, px, py) - offset;

/***************************************************************************
      Check whether star is enough within image to produce target stamp */
      regchk = regionCheck(star, px, py);
      switch (regchk)
      {
        case 1: {                /* Star not yet entered image domain */
          p1[it] = 0; p2[it] = 0; 
          continue;                           
        }
        case 2: {                /* Star has left image domain */
          printf("\n***Star has left domain: CASE 2***\n\n");
          fits_close_file(infptr, &status);     /*  Close fits file */
          if (status) fits_report_error(stderr, status);    /* print any error message */
          printf("\n\n***Fitsfile closed: CASE 2***\n");
          break;                              
        }
      }

/************************
      Close fits file   */
      printf("\n\n***Closing fitsfile***\n");
      fits_close_file(infptr, &status);     /*  Close fits file */
      printf("\n\n***Closed fitsfile***\n");
    }
    else{
      if (status) fits_report_error(stderr, status);    /* print any error message */
      printf("\n\n***Missing fits file***\n%s\n\n",file_name);
      status = 0;   /* Reset status to zero */
      continue;
    }               /* End of if statement opening input image fits file */

/**********************************
    Produce target image stamp   */  

    produceStamp(image, px, py, stamp);
    for(jj = 0; jj < N_STAMP; jj++){
      for(ii = 0; ii < N_STAMP; ii++){
        s[it][jj][ii] = stamp[ii][jj];    /* Note reversal of s dimensions */
      }  
    }
    printf("\nSaved target image to array\n");

    printf("\n");
  }                  /* End of for loop processing a single image */

  printf("\nOutOfTheLoop\n");
  strcpy(outputfilename, "!writeTestBinaryTable.fits");

  fits_create_file(&outptr, outputfilename, &status); /* create new file */
  if (status) fits_report_error(stderr, status);    /* print any error message */
  status = 0;

  fits_create_tbl(outptr, BINARY_TBL, TL, n_cols, ttype, tformb, tunit, extname, &status);
  if (status) fits_report_error(stderr, status);    /* print any error message */
  status = 0;


/* BJD TIME column and related keys */

  /* Assign TIME to 1st column */  colnum = 1;  firstrow = 1;  firstelem = 1;  nelements = TL;  fits_write_col(outptr, TDOUBLE, colnum, firstrow, firstelem, nelements, BJD, &status);
  if (status) fits_report_error(stderr, status);    /* print any error message */
  status = 0;

  fits_update_key(outptr, TSTRING, "TDISP1", "D14.7", "column display format", &status);
  if (status) fits_report_error(stderr, status);    /* print any error message */
  status = 0;

/* TARGET IMAGE STAMP column and related keys */

  /* Assign STAMP image to 2nd column */  colnum = 2;  firstrow = 1;  firstelem = 1;  nelements = N_STAMP*N_STAMP*TL;  fits_write_col(outptr, TFLOAT, colnum, firstrow, firstelem, nelements, s, &status);
  if (status) fits_report_error(stderr, status);    /* print any error message */
  status = 0;

  fits_update_key(outptr, TSTRING, "TDIM2", "(10,10)", "Image stamp dimension", &status);
  if (status) fits_report_error(stderr, status);    /* print any error message */
  status = 0;

/* X-PIXEL column and related keys */

  /* Assign x-pixel to 3rd column */  colnum = 3;  firstrow = 1;  firstelem = 1;  nelements = TL;  fits_write_col(outptr, TUINT, colnum, firstrow, firstelem, nelements, &p1, &status);
  if (status) fits_report_error(stderr, status);    /* print any error message */
  status = 0;

/* Y-PIXEL column and related keys */

  /* Assign y-pixel to 4th column */  colnum = 4;  firstrow = 1;  firstelem = 1;  nelements = TL;  fits_write_col(outptr, TUINT, colnum, firstrow, firstelem, nelements, &p2, &status);
  if (status) fits_report_error(stderr, status);    /* print any error message */
  status = 0;

  for(ii = 0; ii < TL; ii++){
    rr = TL - ii - 1;

//    printf("\nrow:%d\tpx:%d",rr, p1[rr]);

    if (p1[rr] == 0){
      fits_delete_rows(outptr, rr+1, 1, &status);
    }
  }
  printf("\n");

/* Additional keys */

  /* Write header keywords; must pass the ADDRESS of the value */  fits_update_key(outptr, TINT, "STATUS", &status, "File open status", &status);
  if (status) fits_report_error(stderr, status);    /* print any error message */
  status = 0;

  fits_update_key(outptr, TUSHORT, "N_ARR", &n_cols, "Number of arrays", &status);
  if (status) fits_report_error(stderr, status);    /* print any error message */
  status = 0;

  fits_close_file(outptr, &status);     /*  Close fits file */
  if (status) fits_report_error(stderr, status);    /* print any error message */
  status = 0;

    return 0;
}
