#include "./headers/createTPFs.h"

int main(int argc, char *argv[])
{

/*****************************************************************************************/
/* Main program to read HI-1A images from a series of .fits files during a given orbit   */
/* for a selected star.                                                                  */
/* The star is tracked as it moves across the HI-1A images.                              */
/* A 20x20 pixel sub-image, or 'stamp', centered on the center of the stellar image is   */
/* extracted for each HI-1A image, forming a time-series of stamps in the form of a      */
/* time-stamped data-cube.                                                               */
/* Along with the HI-1A pixel coordinates of the centroid of the stellar image, this     */
/* data is output to the fits file, along with relevant meta-data within the file header */
/*****************************************************************************************/

/*
  ORBIT variable initialization */
  int orbit;                         /* Orbit number (see tables/orbit_0h_cross_times.txt) */
  int first_orb_arg = 11;            /* Index of first orbit argument (for setting up orbit number array) */
  int Norbs = argc - first_orb_arg;  /* Number of orbits */
  int orbits[Norbs];                 /* Array containing orbit numbers */
                                     /* Using malloc, etc. gave strange results */

/*
  TIME variable initialization */
//  const int t_str = 541;             /* Time of first image of the day in seconds hhmmss: 000901 (not 2007, 2021) */
  const int dt = 40;                 /* Time between images in minutes (image cadence) */
  const int ipd = 36;                /* Number of images per day */
  const int TL = 100*ipd;            /* Limiting number of images to acquire (over-estimates number needed) */
  int it;                            /* Image timestamp index */
  int date_ymd[3], time_hms[3];      /* Date array: [y m d], UTC Time array: [h m s] */

/*
  INPUT FILE arrays initialization */
  fitsfile *infptr;                  /* Pointer to HI-1A input image FITS file, defined in fitsio.h */
  char file_name[120];               /* Filename of current HI-1A fits image */

/*
  IMAGE array initialization */
  float image[N_AXES][N_AXES];       /* HI-1A full image array */
  float *ptr_image = &image[0][0];   /* Pointer to image array */

/*
  STAMP array initialization */
//  float stamp[N_STAMP][N_STAMP];     /* Star single image stamp array */

/*
  SURVEY arrays initialization */
  double BJD[TL];                    /* Barycentric Julian Date time */
  const double offset = 2450000.0;   /* BJD offset */
  float s[TL][N_STAMP][N_STAMP];     /* Target imagestamps - Note reversal of dimensions */
  float *ptr_s = &s[0][0][0];        /* Pointer to complete image stamp array */
  double px[TL], py[TL];             /* x, y image coordinates of star */
  char datetimestr[TL][DTS_SIZE];    /* Date & time information relating to input image filename */
                                     /* (yyyymmdd_hhmmss) */

/*
  OUTPUT FITS FILE variable initialization */
//  fitsfile *outptr;                  /* Pointer to HI-1A output TPF FITS file, defined in fitsio.h */
//  char outputfile[60];               /* TPF filename to save */
//  char outputfilename[120];          /* Directory path to save + TPF filename */

  /*
  OTHER variable initializations */
  int regchk;                        /* Flag to check if star exists in image */
  int miss, N_good_img;              /* Number of missing and good images */
  int status = 0;                    /* CFITSIO status value MUST be initialized to zero! */

  TARG star;                         /* Star characteristics structure */
  PTH dir;                           /* Directory paths structure */

/********************************************
  Distribute input arguments to variables   */

  distArgs(argc, argv, &star, &dir, orbits, Norbs);

/**************************************************
  Convert celestial coordinates pair to degrees   */

  star.RAdeg  = RAhms2deg(star.RAhms);
  star.DECdeg = DECdms2deg(star.DECdms);

/****************************
 Loop over range of orbits  */

  for (int io = 0; io < Norbs; io++)
  {
    orbit = orbits[io];
    printf("\nProcessing orbit number: %d\n\n",orbit);

    memset(BJD, 0, sizeof(BJD));   /* Initialize all BJD times to zero */
    memset(px, 0, sizeof(px));     /* Initialize all x-pixel position values to zero */
    memset(py, 0, sizeof(py));     /* Initialize all y-pixel position values to zero */

    miss = 0; N_good_img = 0;      /* Reinitialize missing & good image counts */
    status=0;                      /* Reset CFITSIO status to zero */

/***************************************
  Calculate analysis start date & time */

    if (getStartDate(orbit, star.RAdeg, date_ymd, dir.base) < 0)
    {
      break;
    }

/***********************
    Loop over images   */

    for (it = 0; it < TL; it++)
    {
      printf("\n***************************************************************************\n");

/******************************************
    Calculate current image date/time and place into respective arrays */
      if (image_date_time(date_ymd, time_hms, dt, it) < 0)
      {
        break;
      }

/******************************************
    Produce HI-1A image FITS path/filename string from date/time */
      createFilename(it, date_ymd, time_hms, dir.HI1A_data, file_name, datetimestr[it]);

/*******************************************
    Test then open HI-1A image fits file   */

      if (!fits_open_file(&infptr, file_name, READONLY, &status)){

/******************************************
      Acquire full image from HI-1A image fits file   */

        acquireImage(infptr, ptr_image);

/***************************************************************************
      Find star's image pixel coordinates from its celestial coordinates and
      Check whether star is enough within image (> 30 pix from edge)
      to produce target stamp */

        regchk = coords2pixels(infptr, &star, &px[it], &py[it]);

/************************
      Close HI-1A image fits file   */

        fits_close_file(infptr, &status);

/************************
      Control program flow depending on pixel coordinates of star with respect to HI-1A image   */

        if (regchk != 0)            /* Star is not within image domain */
        {
          miss++;                  /* Increment missing image count */
          status = 0;              /* Reset CFITSIO status to zero */
          if (regchk == 1)         /* Star has not yet reached image domain */
          {
            continue;              /* Jump to next image acquisition loop */
          }
          else if (regchk == 2)    /* Star has left image domain */
          {
            break;                 /* Leave image acquisition loop */
          }
        }

/************************
      Calculate Barycentric Julian Day (minus offset) for given date & time   */

        BJD[it]=STEREO_UTC2BJD(date_ymd, time_hms, &star, dir.base) - offset;

      }               /* End of if statement opening input image fits file */
      else            /* If opening input image fits file fails */
      {
        printf("\n***Missing fits file*** (%s)\n",file_name);
        miss++;       /* Increment missing image count */
        status = 0;   /* Reset CFITSIO status to zero */
        continue;     /* Jump to next image acquisition loop */
      }               /* End of else statement for missing input image fits file */

/**********************************
    Produce target image stamp   */

    /* Passes pointer to beginning of stamp array for current time iteration */
      produceStamp(ptr_image, (int) floor(px[it]), (int) floor(py[it]), &s[it][0][0]);
      printf("\nSaved target image to array.\n");

      N_good_img++;   /* Increment good image count */

    } /* End of 'it' time itertation loop processing single images into pixel stamps */

    printf("\n***************************************************************************\n");
    printf("\nMissing files:%d\n",miss);
    printf("Good images:%d\n\n",N_good_img);

/******************************
  Produce output fitsfile   */

    output2fits(TL, BJD, ptr_s, N_good_img, px, py, datetimestr, &star, &dir, orbit);

  } /* End of for 'orbit' loop processing a single image */

  return 0;

} /* End of program */
