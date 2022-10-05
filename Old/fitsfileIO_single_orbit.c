#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <dirent.h>
#include <unistd.h>
#include "fitsio.h"

#define PI 3.141592654
#define N_AXES 1024
#define N_STAMP 20

int getStarCoords(int, char *filename, char *star, double *coords);
int getStartDate(int orbit, double *coords, int *date_ymd);
void createFilename(int *date_ymd, int *time_hms, char *file_name, char s_datetime[]);
void image_date_time(int *date_ymd, int *time_hms);
void acquireImage(fitsfile *, char *, float *image, int);
void coords2pixels(fitsfile *, double *coords, double *, double *);
double STEREO_UTC2BJD(int *date_ymd, int *time_hms, double, double, double *coords);
int regionCheck(char *star, double, double, int, int);
void produceStamp(float *image, int, int, float *stamp, int, int);
void output2fits(char *filename, int , double *BJD, int , float *s, int , double *p1, double *p2, char dtstr[][16]);


int main(){

/****************************************************************************/
/* Main program to read in an image from a hard-coded, selected .fits file  */
/* for a hard-coded star. Flux values for a pixel can be found.             */
/****************************************************************************/

/*
  STAR variable initialization */
  double coords[6];
  char star[30];
//  const char coorfilename[] = "MyStars_h1A_K2-EPIC_details.txt";
  char coorfilename[] = "/Users/pwilliams/Documents/Programming/C/Projects/c/Tests/fitsfileIO/OneStar.txt";
//  char coorfilename[] = "/Users/pwilliams/Documents/Programming/C/Projects/c/Tests/fitsfileIO/MyStarsTest.txt";
//  const char coorfilename[] = "MyStars_h1A_K2-EPIC_details_chunk.txt";
//  const char coorfilename[] = "starCoords.txt";
  const char stereo_star_dir[] = "/Users/pwilliams/Documents/Research/stereo/StellarData";
  int is;

/*
  INPUT FILE arrays initialization */
  int orbit;
  int date_ymd[3], time_hms[3];
  char file_name[100];

/*
  TIME variable initialization */
  int t_str = 541;                  /* Time of first image of the day in seconds hhmmss: 000901 (not 2007) */
  int dt = 40*60;                   /* Time between images in seconds (40 minute cadence) */
  int ipd = 36;                     /* Number of images in a day */
  int t_end = t_str + dt*(ipd-1);   /* Time of first image of the day in seconds */
  int it;
  char s_datetime[20];

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
  double px=0.0, py=0.0;
  float stamp[N_STAMP][N_STAMP];
  float *ptr_stamp;

/*
  OTHER variable initialization (for loops, etc.) */
  int ii, jj, rr, tt, regchk, miss, N_good_img, TL = 100*ipd;

/*
  SURVEY arrays initialization */
  double offset = 2450000.0;      /* BJD offset */
  unsigned short t[TL];           /* time */
  double p1[TL], p2[TL];          /* x- & y-pixels */
  float s[TL][N_STAMP][N_STAMP];  /* target imagestamps - Note reversal of dimensions */
  double BJD[TL];                 /* BJD time */
  char datetimestr[TL][16];       /* Date & time information relating to input image filename */

/*
  OUTPUT FITS FILE variable initialization */
  char outputfile[100];
  char outputfilename[120];


/***************************************************************************/

  orbit = 10;

/***************************************************************************/

  for (is = 0; is < 1; is++){   /* Loop through star list */
//  for (is = 0; is < 8; is++){   /* Loop through star list */

  miss = 0; N_good_img = 0;   /* Reinitialize missing & good image counts */
  status=0;

/*********************************************
  Input star name and its celestial coordinates from file */

    if (getStarCoords(is, coorfilename, star, coords) < 0)
      break;

/*************************
  FILE Start date & time */

    if (getStartDate(orbit, coords, date_ymd) < 0)
      break;

    time_hms[0]  = 0;
    time_hms[1]  = 9; 
    time_hms[2]  = 1; 

//    date_ymd[0]  = 2008;     time_hms[0]  = 0; 
//    date_ymd[1]  = 4;        time_hms[1]  = 9;
//    date_ymd[2]  = 2;        time_hms[2]  = 1;
//    date_ymd[0]  = 2016;     time_hms[0]  = 0; 
//    date_ymd[1]  = 5;        time_hms[1]  = 9;
//    date_ymd[2]  = 25;        time_hms[2]  = 1;
//    date_ymd[0]  = 2021;     time_hms[0]  = 0; 
//    date_ymd[1]  = 6;        time_hms[1]  = 9;
//    date_ymd[2]  = 1;        time_hms[2]  = 1;
//    date_ymd[1]  = 6;        time_hms[1]  = 9;
//    date_ymd[2]  = 4;        time_hms[2]  = 1;
//    date_ymd[2]  = 10;        time_hms[2]  = 1;

    if (date_ymd[0] > 2020 & date_ymd[1] > 5)
    {
        time_hms[1]  = 8;
        time_hms[2]  = 31;
    }

    for (it = 0; it < TL; it++){
      printf("\n***************************************************************************\n");

      if (it > 0)
        image_date_time(date_ymd, time_hms);

      t[it] = it;

      createFilename(date_ymd, time_hms, file_name, s_datetime);
      printf("\n%d:\t%s\n",t[it],file_name);
      printf("\n%s\n",s_datetime);

      strcpy(datetimestr[it],s_datetime);
//      datetimestr[it]=s_datetime;
      printf("\n%d:\t%s\n",t[it],s_datetime);
      printf("\n%d:\t%s\n",t[it],datetimestr[it]);

/*******************************
    Test then open fits file   */
//      printf("\nfile_name size:\t%lu\n\n",sizeof(file_name));
//      printf("\ninfptr size:\t%lu\n\n",sizeof(infptr));
//      printf("\ninfptr:\t%p\n\n",infptr);

      if( access( file_name, F_OK) == 0)
      {
        printf("File exists.\n");
      }
      else
      {
        printf("File doesn't exist.\n");
      }

      if (!fits_open_file(&infptr, file_name, READONLY, &status)){
//      if( access( file_name, F_OK) == 0) {
//        fits_open_file(&infptr, file_name, READONLY, &status);
/******************************************
      Acquire full image from fits file   */

        ptr_image = &image[0][0];
        acquireImage(infptr, file_name, ptr_image, N_AXES);
//        printf("\nimage size:\t%lu\n\n",sizeof(image));

/***************************************************************************
      Find star's image pixel coordinates from its celestial coordinates   */

        coords2pixels(infptr, coords, &px, &py);
        p1[it] = px; p2[it] = py;
//        BJD[it]=0.0;    /* Sets a flag to ignore/remove bad data */
        BJD[it]=STEREO_UTC2BJD(date_ymd, time_hms, px, py, coords) - offset;
//        printf("\nBJD size:\t%lu\n\n",sizeof(BJD));

/***************************************************************************
      Check whether star is enough within image to produce target stamp */

        regchk = regionCheck(star, px, py, N_AXES, N_STAMP);

        if (regchk == 1)         /* Star not yet entered image domain */
        {
          BJD[it]=0.0;    /* Sets a flag to ignore/remove bad data */
          miss += 1;
          status = 0;
          continue;       /* Jump to next data acquisition loop */
        }
        else if (regchk == 2)    /* Star has left image domain */
        {                
          BJD[it]=0.0;    /* Sets a flag to ignore/remove bad data */
          fits_close_file(infptr, &status);  /* No more data to collect - close fits file */
          miss += 1;
          status = 0;
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

      produceStamp(ptr_image, (int) floor(px), (int) floor(py), ptr_stamp, N_AXES, N_STAMP);
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
    printf("Good images:%d\n\n\n",N_good_img);

    for (int iit = it; iit < TL; iit++)
        BJD[iit]=0.0;


/******************************
  Produce output fitsfile   */  

  /* Create filename (! means to overwrite existing file) */
    sprintf(outputfile, "STEREO_HI-1A_r%2d_%s.fits", orbit, star);
//    sprintf(outputfilename, "!writeTestBinaryTable_%s.fits", star);

    sprintf(outputfilename,"!%s/%s/%s",stereo_star_dir,star,outputfile);


    output2fits(outputfilename, TL, BJD, N_STAMP, &s[0][0][0], N_good_img, p1, p2, datetimestr);

  }

//  printf("\n\nStar: %s\n\n",star);
//  for (it = 0; it < TL; it++){
//      printf("%d\t%12.7f\t%6.2f\t%6.2f\t%s\n",it+1,BJD[it],p1[it],p2[it],datetimestr[it]);
//  }

  return 0;

} /* End of program */
