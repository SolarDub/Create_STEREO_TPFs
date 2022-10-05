#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <dirent.h>
#include "fitsio.h"
#include "./headers/constants.h"
#include "./headers/struct_target.h"

int regionCheck(char *star, double px, double py);

/********************************************************************************************/
/*  Function:                                                                               */
/*    - 'coords2pixels'                                                                     */
/*  Inputs:                                                                                 */
/*    - fptr:      File pointer to HI-1A image FITS file                                    */
/*    - ptr_star:        Pointer to star properties structure                               */
/*      - RAdeg:           Right Ascension in degrees                                       */
/*      - DECdeg:          Declination in degrees                                           */
/*    - pxvl:      Initially empty pointer to x-pixel position of star in HI-1A image       */
/*    - pyvl:      Initially empty pointer to y-pixel position of star in HI-1A image       */
/*  Output:                                                                                 */
/*    - pxvl:      Pointer to calculated x-pixel position of star in HI-1A image            */
/*    - pyvl:      Pointer to calculated y-pixel position of star in HI-1A image            */
/********************************************************************************************/

int coords2pixels(fitsfile *fptr, TARG *ptr_star, double *pxvl, double *pyvl)
{

/***************************************************************************/
/* Header file to read in an image from a hard-coded, selected .fits file  */
/* for source with celestial coordinates, RA and DEC.                      */
/* The program will then calculate the image pixel coordinates, px & py.   */
/***************************************************************************/

/*
  STAR celestial coordinates in radians */
  double RA  = (PI/180.0)*(ptr_star->RAdeg);
  double DEC = (PI/180.0)*(ptr_star->DECdeg);

/*
  IMAGE variable initialization */
  int status = 0;   /* CFITSIO status value MUST be initialized to zero! */

/*
  KEYWORD variable initialization */
  char keyname[10];
  char keyval[80], comm[80];
  double RAp, DECp, phip, RApr, DECpr, phipr;
  double rx, ry, sx, sy, pc11A, pc12A, pc21A, pc22A;
  char *projx, *piece, *proj;

/*
  COORDINATE TRANSFORMATION variable initialization */
  double sinth, costh, theta, sinphi, cosphi;
  double gamma, mu, R, x, y, detA, vpx, vpy;
  double pxrm, pyrm, pxdiff, pydiff;
  int ppx, ppy;

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


  /* sx (CDELT1A) and sy (CDELT2A) pixel width and height (deg per pixel) */
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
/* after Greisen & Calabretta (2002) A&A, 395, 1061 (GC)                      */
/* and   Calabretta & Greisen (2002) A&A, 395, 1077 (CG)                      */
/******************************************************************************/

  /* Perform spherical coordinate transformation from celestial to native coordinates */
  /* - following CG Eq (A.1) */

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


  /* Perform zenithal (azimuthal) perspective projection (CG Sec. 5.1.1) */
  if (strncmp(proj,"AZP",3))
  {
    gamma = 0;

  /* mu (PV2_1A) conic mid-latitude of image (deg) */
  /* mu is the number of spherical radii from sphere center to projection point */
    strcpy(keyname,"PV2_1A");
    fits_read_keyword(fptr, keyname, keyval, comm, &status);
    mu = atof(keyval);

    R = (180.0/PI)*(mu + 1)*cos(theta)\
      / (mu+sin(theta)+cos(theta)*cosphi*tan(gamma));    /* CG Eq (16) */

    x =  R*sinphi;                 /* CG Eq (20) */
    y = -R*cosphi/cos(gamma);      /* CG Eq (21) */
  }

  /* Perform image rotation and scaling */
  /* from intermediate world coordinates (x,y) while scaling (sx, sy) */
  /* to image pixel number (vpx, vpy) */
  /*    let:  A = [pc11A,pc12A;pc21A,pc22A]
           vx = [x;y]      (in degrees)
           vs = [sx;sy]    (in degrees per pixel)

     then: vp = inv(A)(vx./vs)   (element-wise division)
    where: vp = [vpx;vpy]              */

  /* Refer to GC eq (3) (or CG eq (1)) and perform inverse process */

  detA = (pc11A*pc22A) - (pc12A*pc21A);

  vpx = (( pc22A*x/sx) - (pc12A*y/sy))/detA;   /* x-pix position from image center */
  vpy = ((-pc21A*x/sx) + (pc22A*y/sy))/detA;   /* y-pix position from image bottom */


  /* Determine pixel coordinates from bottom-left of image */
  *pxvl = vpx + rx;  /* x-pixel coordinate from left of image (float) */
  *pyvl = vpy + ry;  /* y-pixel coordinate from bottom of image (float) */

  /* x- and y-pixel decimal remainder (float) */
  pxrm = *pxvl - floor(*pxvl);
  pyrm = *pyvl - floor(*pyvl);

  /* x- and y-pixel coordinate (integer) */
  ppx = round(*pxvl);
  ppy = round(*pyvl);

  /* x- and y-offset of pixel position from integer pixel value (float) */
  pxdiff = *pxvl - ppx;
  pydiff = *pyvl - ppy;

  if (ppx > 0)
    printf("\nPixel Coordinates:\tpx: %d\tpy %d\t(%f,%f )\n", ppx, ppy, *pxvl, *pyvl);

  if (status) fits_report_error(stderr, status); /* print any error message */

  return regionCheck(ptr_star->name, *pxvl, *pxvl);

}

/********************************************************************************************/

int regionCheck(char *star, double px, double py)
{

/***************************************************************************/
/* Checks whether target star is enough within image to make target stamp  */
/***************************************************************************/

  if ((px < N_STAMP + 10) || (py < N_STAMP + 10))
  {
    printf("\n%s not yet reached image region of interest. SKIP TO NEXT LOOP.\n", star);
    return(1);
  }
  else if ((px > N_AXES - (N_STAMP + 10)) || (py > N_AXES  - (N_STAMP + 10)))
  {
    printf("\n%s has left the image region of interest. END OF LOOP.\n", star);
    return(2);
  }
  return(0);
}
