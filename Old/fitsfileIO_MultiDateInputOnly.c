#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <dirent.h>
#include "fitsio.h"
#include "./include/coords2pixels.h"
#include "./include/acquireImage.h"
#include "./include/miniFuncs.h"

#define PI 3.141592654
#define N_AXES 1024
#define N_STAMP 10

void concatCoords(char *star, float RAhms[], float DEChms[], float coords[]);
void createFilename(int date_ymd[], int time_hms[], char *file_name);
void prepNext(int date_ymd[], int time_hms[]);
void acquireImage(fitsfile *, char *, float image[][N_AXES]);
void coords2pixels(fitsfile *, float coords[], int *, int *);
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
  int px=0, py=0;

/*
  FILE variable initialization */
  int date_ymd[3], time_hms[3];
  char file_name[100];

/*
  TIME variable initialization */
  int t_str = 541; /* Time of first image of the day in seconds hhmmss: 000901 (not 2007) */
  int dt = 40*60;  /* Time between images in seconds (40 minute cadence) */
  int ipd = 36;  /* Number of images in a day */
  int t_end = t_str + dt*(ipd-1); /* Time of first image of the day in seconds */
  int it;

/*
  FITSFILE variable initialization */
  fitsfile *fptr;         /* FITS file pointer, defined in fitsio.h */
  int status = 0;   /* CFITSIO status value MUST be initialized to zero! */

/*
  IMAGE variable initialization */
  float image[N_AXES][N_AXES];

/*
  STAMP variable initialization */
  float stamp[N_STAMP][N_STAMP];

/*
  OTHER variable initialization (for loops, etc.) */
  int ii, jj;

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
  date_ymd[2]  = 2;        time_hms[2]  = 1;

  for (it = 0; it < ipd+5; it++){

    createFilename(date_ymd, time_hms, file_name);

    prepNext(date_ymd, time_hms);

/*******************************
    Test then open fits file   */

    if (!fits_open_file(&fptr, file_name, READONLY, &status)){

/******************************************
      Acquire full image from fits file   */

      acquireImage(fptr, file_name, image);

/***************************************************************************
      Find star's image pixel coordinates from its celestial coordinates   */

      coords2pixels(fptr, coords, &px, &py);

/***************************************************************************
      Check whether star is in enough within image to produce target stamp */

      switch (regionCheck(star, px, py))
      {
        case 1: continue;                           /* Star not yet entered image domain */
        case 2: break;                              /* Star has left image domain */
      }

/************************
      Close fits file   */

      fits_close_file(fptr, &status);     /*  Close fits file */
    }
    else{
      if (status) fits_report_error(stderr, status);    /* print any error message */
      printf("\n\n***Missing fits file***\n%s\n\n",file_name);
      status = 0;   /* Reset status to zero */
      continue;
    }

/**********************************
    Produce target image stamp   */  

    produceStamp(image, px, py, stamp);

    printf("\n(0,0):\t%f\n(9,9):\t%f\n",stamp[0][0],stamp[9][9]);

    printf("\n");
  }
    return 0;
}
