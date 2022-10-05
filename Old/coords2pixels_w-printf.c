#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "fitsio.h"

#define PI 3.141592654

int main(){

/*
  STAR variable initialization */
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
  int px, py;


/***************************************************************************/
/****************************************************************************
  STAR celestial coordinates */
  RAhms[0] = 3;       DEChms[0] =  23;
  RAhms[1] = 46;      DEChms[1] =  56;
  RAhms[2] = 19.574;  DEChms[2] =  54.080;

/* Convert to degrees */
  RA_deg  = 15*(RAhms[0] + (RAhms[1] + (RAhms[2]/60.0))/60.0);
  DEC_deg = DEChms[0] + (DEChms[1] + (DEChms[2]/60.0))/60.0;

/* Convert to radians */
  RA =  (PI/180.0)*RA_deg;
  DEC = (PI/180.0)*DEC_deg;

  printf("\nRA: %2.0fh %2.0fm %6.3fs DEC: %2.0fd %2.0fm %6.3fs\n"\
        , RAhms[0],  RAhms[1],  RAhms[2]\
        , DEChms[0], DEChms[1], DEChms[2]);
  printf("\nRA  (deg): %f\t(rad): %f",RA_deg, RA);
  printf("\nDEC (deg): %f\t(rad): %f\n",DEC_deg, DEC);



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
    printf("\nproj: %s", proj);

  /* RA (CRVAL1A) and DEC (CRVAL2A) at image center (deg) */
    strcpy(keyname,"CRVAL1A");
    fits_read_keyword(fptr, keyname, keyval, comm, &status);
    RAp = atof(keyval);
    RApr = (PI/180.0)*RAp;   /* Convert to radians */
    printf("\nRAp: %f  \tRAPpr: %f", RAp, RApr);

    strcpy(keyname,"CRVAL2A");
    fits_read_keyword(fptr, keyname, keyval, comm, &status);
    DECp = atof(keyval);
    DECpr = (PI/180.0)*DECp;   /* Convert to radians */
    printf("\nDECp: %f  \tDECpr: %f", DECp, DECpr);
   
  /* phip (LONPOLE) in native coord system (deg) */
    strcpy(keyname,"LONPOLE");
    fits_read_keyword(fptr, keyname, keyval, comm, &status);
    phip = atof(keyval);
    phipr = (PI/180.0)*phip;   /* Convert to radians */
    printf("\nphip: %f\tphipr: %f", phip, phipr);

  /* rx (CRPIX1A) and ry (CRPIX2A) x- and y-pixel values at image center (pix) */
    strcpy(keyname,"CRPIX1A");
    fits_read_keyword(fptr, keyname, keyval, comm, &status);
    rx = atof(keyval);
    strcpy(keyname,"CRPIX2A");
    fits_read_keyword(fptr, keyname, keyval, comm, &status);
    ry = atof(keyval);
    printf("\nrx: %f\try: %f", rx, ry);

  /* sx (CDELT1A) and sy (CDELT2A) pixel width and height (deg) */
    strcpy(keyname,"CDELT1A");
    fits_read_keyword(fptr, keyname, keyval, comm, &status);
    sx = atof(keyval);
    strcpy(keyname,"CDELT2A");
    fits_read_keyword(fptr, keyname, keyval, comm, &status);
    sy = atof(keyval);
    printf("\nsx: %f\tsy: %f", sx, sy);

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
    printf("\nPC matrix:\n%f\t%f\n%f\t%f", pc11A, pc12A, pc21A, pc22A);

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

    printf("\nLAT:\n%f\t%f\t%f\n", sinth, costh, theta);

  /* Find native longitude coordinate */
    cosphi = ((-sin(RApr)*sin(phipr)-cos(RApr)*cos(phipr)*sin(DECpr))*cos(DEC)*cos(RA)\
           +  ( cos(RApr)*sin(phipr)-sin(RApr)*cos(phipr)*sin(DECpr))*cos(DEC)*sin(RA)\
           +  ( cos(phipr)*cos(DECpr))*sin(DEC))
           /  costh;

    sinphi = (( sin(RApr)*cos(phipr)-cos(RApr)*sin(phipr)*sin(DECpr))*cos(DEC)*cos(RA)\
           +  (-cos(RApr)*cos(phipr)-sin(RApr)*sin(phipr)*sin(DECpr))*cos(DEC)*sin(RA)\
           +  ( sin(phipr)*cos(DECpr))*sin(DEC))
           /  costh;

    printf("\nLON:\n%f\t%f\n", cosphi, sinphi);

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

      printf("\nmu: %f\t  R: %f", mu, R);
      printf("\nx:  %f\t  y: %f", x, y);
    }

/*    let:  A = [pc11A,pc12A;pc21A,pc22A]
           vx = [x;y]
           vs = [sx;sy]

     then: vp = inv(A)(vx./vs);
 */
    detA = (pc11A*pc22A) - (pc12A*pc21A);

     vpx = (( pc22A*x/sx) - (pc12A*y/sy))/detA;
     vpy = ((-pc21A*x/sx) + (pc22A*y/sy))/detA;

     printf("\nvpx:  %f\t  vpy: %f", vpx, vpy);

     pxvl = vpx + rx;  /* x-pixel coordinate from left of image (float) */
     pyvl = vpy + ry;  /* y-pixel coordinate from bottom of image (float) */

     /* x- and y-pixel decimal remainder (float) */
     pxrm = pxvl - floor(pxvl);
     pyrm = pyvl - floor(pyvl);

     /* x- and y-pixel coordinate (integer) */
     px = round(pxvl);
     py = round(pyvl);

     /* x- and y-offset of pixel position from integer pixel value (float) */
     pxdiff = pxvl - px;
     pydiff = pyvl - py;

     printf("\npxvl:  %f\t  pyvl: %f", pxvl, pyvl);
     printf("\npxrm:  %f\t  pyrm: %f", pxrm, pyrm);
     printf("\npx:  %d\t  py: %d", px, py);
     printf("\npxdiff:  %f\t  pydiff: %f", pxdiff, pydiff);


/*******************
  Close fits file */
   fits_close_file(fptr, &status);
  }

  printf("\n");

  if (status) fits_report_error(stderr, status); /* print any error message */
  return (status);}