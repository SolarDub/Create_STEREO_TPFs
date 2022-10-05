#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "fitsio.h"

#define N_AXES 1024
#define PI 3.141592654

void coords2pixels(int *, int *);

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
  float RAhms[3], DEChms[3];
  char star[30];
  int px=0, py=0;

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

   coords2pixels(&px,&py);

   printf("\npx: %d\tpy %d\n", px, py);

/*******************
  Close fits file */
    fits_close_file(fptr, &status);
  }

  printf("\n");

  if (status) fits_report_error(stderr, status); /* print any error message */
  return (status);
}

void coords2pixels(int *ppx, int *ppy)
{

/***************************************************************************/
/* Self-contained program to read in an image from a hard-coded, selected  */
/* .fits file for a hard-coded star with celestial coordinates, RA and DEC.*/
/* The program will then calculate the image pixel coordinates, px & py.   */
/***************************************************************************/

/*
  STAR variable initialization */
  char star[30];
  float RAhms[3] , RA_deg,  RA;
  float DEChms[3], DEC_deg, DEC;

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
  int bitpix;
  long naxes[2] = {1, 1};

/*
  KEYWORD variable initialization */
  char keyname[10];
  char keyval[80], comm[80];
  float RAp, DECp, phip, RApr, DECpr, phipr;
  float rx, ry, sx, sy, pc11A, pc12A, pc21A, pc22A;
  char *projx, *piece, *proj;

/*
  COORD TRANSFORMATION variable initialization */
  float sinth, costh, theta, sinphi, cosphi;
  float gamma, mu, R, x, y, detA, vpx, vpy;
  float pxvl, pyvl, pxrm, pyrm, pxdiff, pydiff;

/***************************************************************************/
/***************************************************************************/

/* Select STAR */
  strcpy(star, "23Tau");

  printf("\nStar:  %s\n", star);

/* STAR celestial coordinates */
  RAhms[0] = 3;       DEChms[0] =  23;
  RAhms[1] = 46;      DEChms[1] =  56;
  RAhms[2] = 19.574;  DEChms[2] =  54.080;

/* Convert to degrees */
  RA_deg  = 15*(RAhms[0] + (RAhms[1] + (RAhms[2]/60.0))/60.0);
  DEC_deg = DEChms[0] + (DEChms[1] + (DEChms[2]/60.0))/60.0;

/* Convert to radians */
  RA =  (PI/180.0)*RA_deg;
  DEC = (PI/180.0)*DEC_deg;

  printf("\nRA:  %2.0fh %2.0fm %6.3fs\tDEC:  %2.0fd %2.0fm %6.3fs\n"\
        , RAhms[0],  RAhms[1],  RAhms[2]\
        , DEChms[0], DEChms[1], DEChms[2]);
  printf("RA:  %f deg\tDEC:  %f deg\n",RA_deg, DEC_deg);



/****************************************************************************
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
/****************************************************************************/
/****************************************************************************/


/*******************
  Test then open fits file   */
  if (!fits_open_file(&fptr, file_name, READONLY, &status))
  {

/*******************
  Get keyword values from fits header   */

  /* Coordinate projection: CTYPE1A */
    strcpy(keyname,"CTYPE1A");
    fits_read_keyword(fptr, keyname, keyval, comm, &status);
    projx = keyval;
    piece = strtok(projx,"---");
    piece = strtok(NULL,"---");
    proj = strtok(piece,"'");


  /* RA (CRVAL1A) and DEC (CRVAL2A) at image center (deg) */
    strcpy(keyname,"CRVAL1A");
    fits_read_keyword(fptr, keyname, keyval, comm, &status);
    RAp = atof(keyval);
    RApr = (PI/180.0)*RAp;   /* Convert to radians */

    strcpy(keyname,"CRVAL2A");
    fits_read_keyword(fptr, keyname, keyval, comm, &status);
    DECp = atof(keyval);
    DECpr = (PI/180.0)*DECp;   /* Convert to radians */

   
  /* phip (LONPOLE) in native coord system (deg) */
    strcpy(keyname,"LONPOLE");
    fits_read_keyword(fptr, keyname, keyval, comm, &status);
    phip = atof(keyval);
    phipr = (PI/180.0)*phip;   /* Convert to radians */


  /* rx (CRPIX1A) and ry (CRPIX2A) x- and y-pixel values at image center (pix) */
    strcpy(keyname,"CRPIX1A");
    fits_read_keyword(fptr, keyname, keyval, comm, &status);
    rx = atof(keyval);
    strcpy(keyname,"CRPIX2A");
    fits_read_keyword(fptr, keyname, keyval, comm, &status);
    ry = atof(keyval);


  /* sx (CDELT1A) and sy (CDELT2A) pixel width and height (deg) */
    strcpy(keyname,"CDELT1A");
    fits_read_keyword(fptr, keyname, keyval, comm, &status);
    sx = atof(keyval);
    strcpy(keyname,"CDELT2A");
    fits_read_keyword(fptr, keyname, keyval, comm, &status);
    sy = atof(keyval);


  /* Rotation matrix components */
    strcpy(keyname,"PC1_1A");
    fits_read_keyword(fptr, keyname, keyval, comm, &status);
    pc11A = atof(keyval);
    strcpy(keyname,"PC1_2A");
    fits_read_keyword(fptr, keyname, keyval, comm, &status);
    pc12A = atof(keyval);
    strcpy(keyname,"PC2_1A");
    fits_read_keyword(fptr, keyname, keyval, comm, &status);
    pc21A = atof(keyval);
    strcpy(keyname,"PC2_2A");
    fits_read_keyword(fptr, keyname, keyval, comm, &status);
    pc22A = atof(keyval);


/******************************************************************************/
/* Change from RA, DEC celestial coordinares to px, py image pixel coordinates*/
/* after Calavretta & Greisen (2002) A&A, 395, 1077                           */
/******************************************************************************/

  /* Find native latitude coordinate */
    sinth = cos(RApr)*cos(DECpr)*cos(DEC)*cos(RA)\
          + sin(RApr)*cos(DECpr)*cos(DEC)*sin(RA)\
          + sin(DECpr)*sin(DEC);

    costh = sqrt(1-(sinth*sinth));

    theta = asin(sinth);


  /* Find native longitude coordinate */
    cosphi = ((-sin(RApr)*sin(phipr)-cos(RApr)*cos(phipr)*sin(DECpr))*cos(DEC)*cos(RA)\
           +  ( cos(RApr)*sin(phipr)-sin(RApr)*cos(phipr)*sin(DECpr))*cos(DEC)*sin(RA)\
           +  ( cos(phipr)*cos(DECpr))*sin(DEC))
           /  costh;

    sinphi = (( sin(RApr)*cos(phipr)-cos(RApr)*sin(phipr)*sin(DECpr))*cos(DEC)*cos(RA)\
           +  (-cos(RApr)*cos(phipr)-sin(RApr)*sin(phipr)*sin(DECpr))*cos(DEC)*sin(RA)\
           +  ( sin(phipr)*cos(DECpr))*sin(DEC))
           /  costh;


    if (strncmp(proj,"AZP",3))
    {
      gamma = 0;

    /* mu (PV2_1A) conic mid-latitude of image (deg) */
    /* mu is the number of spherical radii from sphere center to projection point */
      strcpy(keyname,"PV2_1A");
      fits_read_keyword(fptr, keyname, keyval, comm, &status);
      mu = atof(keyval);

      R = (180.0/PI)*(mu + 1)*cos(theta)\
        / (mu+sin(theta)+cos(theta)*cosphi*tan(gamma));
    
      x =  R*sinphi;
      y = -R*cosphi/cos(gamma);
    }

/*    let:  A = [pc11A,pc12A;pc21A,pc22A]
           vx = [x;y]
           vs = [sx;sy]

     then: vp = inv(A)(vx./vs);
 */
    detA = (pc11A*pc22A) - (pc12A*pc21A);

    vpx = (( pc22A*x/sx) - (pc12A*y/sy))/detA;
    vpy = ((-pc21A*x/sx) + (pc22A*y/sy))/detA;

    pxvl = vpx + rx;  /* x-pixel coordinate from left of image (float) */
    pyvl = vpy + ry;  /* y-pixel coordinate from bottom of image (float) */

    /* x- and y-pixel decimal remainder (float) */
    pxrm = pxvl - floor(pxvl);
    pyrm = pyvl - floor(pyvl);

    /* x- and y-pixel coordinate (integer) */
    *ppx = round(pxvl);
    *ppy = round(pyvl);

    /* x- and y-offset of pixel position from integer pixel value (float) */
    pxdiff = pxvl - *ppx;
    pydiff = pyvl - *ppy;

    printf("\n*ppx:  %d\t  *ppy: %d", *ppx, *ppy);

/*******************
  Close fits file */
   fits_close_file(fptr, &status);
  }

  printf("\n");

  if (status) fits_report_error(stderr, status); /* print any error message */
}