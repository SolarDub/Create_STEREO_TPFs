#include "./headers/output2fits.h"

/********************************************************************************************/
/*  Function:                                                                               */
/*    - 'output2fits'                                                                       */
/*  Inputs:                                                                                 */
/*    - filename:  Output filepath and target pixel FITS filename to write data             */
/*                 (BJD times, image stamps, decimal pixel positions, date/time strings)    */
/*    - TL:        Limiting number of images to acquire (over-estimates number needed)      */
/*    - BJD:       Array containing Barycentric Julian Date times for each image stamp      */
/*    - s:         3-D array containing target pixel images over analyzed time range        */
/*    - Ngood:     Number of good HI-1A images (and thus image stamps)                      */
/*    - p1:        Array containing decimal x-pixel positions of star in HI-1A image        */
/*    - p2:        Array containing decimal y-pixel positions of star in HI-1A image        */
/*    - dtstr:     Array containing date/time strings of form yyyymmdd_hhmmss               */
/********************************************************************************************/

void output2fits(int TL, double *BJD,
                 float *s, int Ngood, double *p1, double *p2, char dtstr[][DTS_SIZE],
                 TARG *ptr_star, PTH *ptr_dir, int orbit)
{

/*
  FITS FILE variable initialization */
  char filename[CHAR_SIZE];
  fitsfile *fptr;               /* FITS file pointer, defined in fitsio.h */
  int status = 0;               /* CFITSIO status value MUST be initialized to zero! */
  int colnum;
//  int hdunum, hdutype, tfields;
//  long nrows;
  long firstrow, firstelem, nelements;

/*
  FITS FILE HEADER variable initialization */
  short n_cols = 5;             /* Number of data columns in fits file */
  char imgstr[5], dimstr[8];
  sprintf(imgstr, "%dE", N_STAMP*N_STAMP);
  sprintf(dimstr, "(%d,%d)", N_STAMP, N_STAMP);

  char *ttype[] = { "TIME", "IMAGE", "X_PIXEL", "Y_PIXEL", "DATE_TIME" }; /* Column titles */
  char *tformb[] = { "D", imgstr, "D", "D","17a"}; /* CFITSIO column formatting codes */
  char *tunit[] = { "BJD - 2450000", "DN/s/CCDPIX", "pixel", "pixel", "date_time" }; /* Column units */
  char extname[] = "TARGETTABLES"; /* extension name */


/*
  FITS FILE DATA variable initialization */
  float s_ave[N_STAMP][N_STAMP];  /* average of target imagestamps */
                                  /* Usage not yet implemented */

/*
  OTHER variable initialization (for loops, etc.) */
  int ii, jj, rr, it;
  char **ptr_dt = malloc(TL * sizeof(char *));  /* Allocate memory for array of pointers */
                                                /* to strings, with TL rows */

/***************************************************************************/

/*
  Allocate memory to datetimestr string pointer rows and copy data into pointer array */
  for (it = 0; it < TL; it++){
    ptr_dt[it] = (char *)malloc(DTS_SIZE+1);   /* Allocate memory to each string row (17 cols)) */
    strcpy(ptr_dt[it],dtstr[it]);
  }

/***************************************************************************/

/* Create filename (! means to overwrite existing file) */
  sprintf(filename,"!%s/%s/STEREO_HI-1A_TPF_r%02d_%s.fits"
                  ,ptr_dir->stereo_star, ptr_star->name, orbit, ptr_star->name);

/***************************************************************************/

  /* Create output fits file */
  fits_create_file(&fptr, filename, &status);        /* create new file */
  if (status) fits_report_error(stderr, status);     /* print any error message */
  status = 0;

  /* Create new binary table extension */
  fits_create_tbl(fptr, BINARY_TBL, TL, n_cols, ttype, tformb, tunit, extname, &status);
  if (status) fits_report_error(stderr, status);    /* print any error message */
  status = 0;


/* BJD TIME column and related keys */

  /* Assign TIME to 1st column */
  colnum = 1;  firstrow = 1;  firstelem = 1;  nelements = TL;
  fits_write_col(fptr, TDOUBLE, colnum, firstrow, firstelem, nelements, BJD, &status);
  if (status) fits_report_error(stderr, status);    /* print any error message */
  status = 0;

  fits_update_key(fptr, TSTRING, "TDISP1", "D14.7", "column display format", &status);
  if (status) fits_report_error(stderr, status);    /* print any error message */
  status = 0;

/* TARGET IMAGE STAMP column and related keys */

  /* Assign STAMP image to 2nd column */
  colnum = 2;  firstrow = 1;  firstelem = 1;  nelements = N_STAMP*N_STAMP*TL;
  fits_write_col(fptr, TFLOAT, colnum, firstrow, firstelem, nelements, s, &status);
  if (status) fits_report_error(stderr, status);    /* print any error message */
  status = 0;

  fits_update_key(fptr, TSTRING, "TDIM2", dimstr, "Image stamp dimension", &status);
  if (status) fits_report_error(stderr, status);    /* print any error message */
  status = 0;

/* X-PIXEL column and related keys */

  /* Assign x-pixel to 3rd column */
  colnum = 3;  firstrow = 1;  firstelem = 1;  nelements = TL;
  fits_write_col(fptr, TDOUBLE, colnum, firstrow, firstelem, nelements, p1, &status);
  if (status) fits_report_error(stderr, status);    /* print any error message */
  status = 0;

  fits_update_key(fptr, TSTRING, "TDISP3", "D7.2", "column display format", &status);
  if (status) fits_report_error(stderr, status);    /* print any error message */
  status = 0;

/* Y-PIXEL column and related keys */

  /* Assign y-pixel to 4th column */
  colnum = 4;  firstrow = 1;  firstelem = 1;  nelements = TL;
  fits_write_col(fptr, TDOUBLE, colnum, firstrow, firstelem, nelements, p2, &status);
  if (status) fits_report_error(stderr, status);    /* print any error message */
  status = 0;

  fits_update_key(fptr, TSTRING, "TDISP4", "D7.2", "column display format", &status);
  if (status) fits_report_error(stderr, status);    /* print any error message */
  status = 0;


  fits_update_key(fptr, TSTRING, "TUNIT5", "yyyymmdd_hhmmss", "column display format", &status);
  if (status) fits_report_error(stderr, status);    /* print any error message */
  status = 0;



/* Date/Time string of H1A respective images */

  /* Assign date/time to 5th column */
  colnum = 5;  firstrow = 1;  firstelem = 1;  nelements = TL;
  fits_write_col(fptr, TSTRING, colnum, firstrow, firstelem, nelements, ptr_dt, &status);
  if (status) fits_report_error(stderr, status);    /* print any error message */
  status = 0;

  free(ptr_dt);   // Free allocated memory

  /* Determine mean image stamp Implementation incomplete.
     Could be used for determining consistent aperture size for performing
     Single Aperture Photometry on image stamps. */
  for (it = 0; it < TL; it++){
    if (BJD[it] != 0.0){
      for(jj = 0; jj < N_STAMP; jj++){
        for(ii = 0; ii < N_STAMP; ii++){
          s_ave[jj][ii]= s_ave[jj][ii]
                       + *(s + it*N_STAMP*N_STAMP + ii*N_STAMP + jj)/Ngood;
        }
      }
    }
    /* Remove null data rows as indicated by zero BJD. */
    rr = TL - it - 1;
    if (BJD[rr] == 0.0){
      fits_delete_rows(fptr, rr+1, 1, &status);
    }
  }

/* Additional keys */

  /* Write header keywords; must pass the ADDRESS of the value */
  fits_update_key(fptr, TINT, "STATUS", &status, "File open status", &status);
  if (status) fits_report_error(stderr, status);    /* print any error message */
  status = 0;

  fits_update_key(fptr, TUSHORT, "N_ARR", &n_cols, "Number of arrays", &status);
  if (status) fits_report_error(stderr, status);    /* print any error message */
  status = 0;

  fits_close_file(fptr, &status);     /*  Close fits file */
  if (status) fits_report_error(stderr, status);    /* print any error message */
  status = 0;

  if (status) fits_report_error(stderr, status); /* print any error message */

}
