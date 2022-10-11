#include "./headers/STEREO_UTC2BJD.h"

/********************************************************************************************/
/*  Function:                                                                               */
/*    - 'STEREO_UTC2BJD'                                                                    */
/*  Inputs:                                                                                 */
/*    - date_ymd:  Pointer to 3-element date array (y, m, d)                                */
/*    - time_hms:  Pointer to 3-element time array (h, m, s)                                */
/*    - px:        Decimal x-pixel position of star in HI-1A image                          */
/*    - py:        Decimal y-pixel position of star in HI-1A image                          */
/*    - ptr_star:        Pointer to star properties structure                               */
/*      - RAdeg:           Right Ascension in degrees                                       */
/*      - DECdeg:          Declination in degrees                                           */
/*    - base_dir:  Directory of executable file                                             */
/*  Output:                                                                                 */
/*    - BJD:       Barycentric Julian Date timestamp of current image                       */
/********************************************************************************************/

/*==========================================================================*/

double STEREO_UTC2BJD(int *date_ymd, int *time_hms, TARG *ptr_star, char *base_dir)
{

/*******************************************************************************************************/
/* Estimate BJD from UTC of photon arrival times from source located at celetsial coordinates (RA,DEC) */
/* Follows techniques described by Eastman, Siverd & Gaudi (2010) PASP, 122, 935                       */
/*******************************************************************************************************/

  const float spd    = 86400.0;            /* Seconds per day */
  const float c      = 299792.458;         /* Speed of light in vacuum in km/s */
  const float DE     = 149597870.700;      /* Earth-Sun distance in km */
  const float RsAU   = 0.9618568;          /* STEREO_A-Sun mean distance in AU */
  const float UTC2TT = 32.184;             /* International Atomic Time (TAI) */
                                           /* to Terrestrial Time (TT) correction */
                                           /* UTC and TAI run at same rate */

  double delSunStA, delSunBary;      /* Distance (in AU) of Sun to STEREO-A and to Solar System Barycenter */
  double RASunStA, DECSunStA;        /* Coordinates from Sun to STEREO-A */
  double RAStASun, DECStASun;        /* Coordinates from STEREO-A to Sun */
  double romer;                      /* Roemer delay */
                                     /* Time difference between photon from an object */
                                     /* to reach the Sun and reach an observer away from the Sun */
  double RASunBary, DECSunBary;      /* Coordinates from Sun to Barycenter */
  double RABarySun, DECBarySun;      /* Coordinates from Barycenter to Sun*/
  double HJD2BJD;                    /* Same as Roemer delay but between the Sun and the Solar System Barycenter */
  double HJD, BJD;                   /* Heliocentric and Barycentric Julian Dates */

/*
  Read in Sun to STEREO-A position ephemeris data from lookup table text file */
  Sun2Pos("StA", 6, date_ymd, time_hms, &RASunStA, &DECSunStA, &delSunStA, base_dir);

  RAStASun = RASunStA + 180;
  RAStASun = RAStASun - 360.0*floor(RAStASun/360.0);

  DECStASun = -DECSunStA;

  romer = delSunStA*(DE/c)*cos((PI/180.0)*(fabs(RAStASun-ptr_star->RAdeg)))
                          *cos((PI/180.0)*(fabs(DECStASun-ptr_star->DECdeg)));
/*
  printf("Dist (AU) of Sun to Stereo A:\t%16.14f\n",delSunStA);

  printf("RA (deg) Sun to Stereo A:\t%11.7f\n",RASunStA);
  printf("RA (deg) Sun to Star:\t%11.7f\n",ptr_star->RAdeg);
  printf("RA (deg) Stereo A to Sun:\t%11.7f\n",RAStASun);

  printf("DEC (deg) Sun to Stereo A:\t%11.7f\n",DECSunStA);
  printf("DEC (deg) Sun to Star:\t%11.7f\n",ptr_star->DECdeg);
  printf("DEC (deg) Stereo A to Sun:\t%11.7f\n",DCStASun);

  printf("\nbeta from horizons: %f deg\n", fabs(RAStASun-(ptr_star->RAdeg)));
  printf("\neps from horizons: %f deg\n", fabs(DECStASun-(ptr_star->DECdeg)));

  printf("\nRomer from horizons: %f sec\n", romer);

  printf("\n\n");
*/
/*
  Calculate Heliocentric Julian Day */
  HJD = JD_GUTC(date_ymd, time_hms) + (UTC2TT + leapSeconds(date_ymd) - romer)/spd;
  /* Romer Delay is subtracted as spacecraft is on opposite side of Sun to star.
  So light reaching barycenter does so about 8 min before reaching spacecraft.
  Second parentheses converted from secs into days with spd.                   */

/*
  Read in Sun to Barycenter position ephemeris data from lookup table text file */
  Sun2Pos("Bary", 7, date_ymd, time_hms, &RASunBary, &DECSunBary, &delSunBary, base_dir);

  HJD2BJD = delSunBary*(DE/c)*cos((PI/180.0)*(fabs(RASunBary-ptr_star->RAdeg)))
                             *cos((PI/180.0)*(fabs(DECSunBary-ptr_star->DECdeg)));

/*
  Calculate Barycentric Julian Day */
  BJD = HJD - HJD2BJD/spd;
  /* if star is on same side of Sun as barycenter, beta < 180, subtract pos HJD2BJD */
  /* if star is on opp side of Sun as barycenter, beta > 180, subtract neg HJD2BJD */
/*
  printf("Dist (AU) of Sun to Barycenter:\t%16.14f\n",delSunBary);

  printf("RA (deg) Sun to Barycenter:\t%11.7f\n",RASunBary);
  printf("RA (deg) Sun to Star:\t%11.7f\n",RAdeg);

  printf("DEC (deg) Sun to Barycenter:\t%11.7f\n",ptr_star->RAdeg);
  printf("DEC (deg) Sun to Star:\t%11.7f\n",ptr_star->DECdeg);

  printf("\nbeta from horizons: %f deg\n", fabs(RASunBary-ptr_star->RAdeg));
  printf("\neps from horizons: %f deg\n", fabs(DECSunBary-ptr_star->DECdeg));

  printf("\nHJD2BJD: %f sec\n", HJD2BJD);

  printf("\nImage date (time):\t%04d/%02d/%02d (%02d:%02d:%02d)\n",
          date_ymd[0], date_ymd[1], date_ymd[2],
          time_hms[0], time_hms[1], time_hms[2]);

  printf("Heliocentric:\t%12.7f\n",HJD);
  printf("HJD2BJD:\t%12.7f\n",HJD2BJD);
  printf("Barycentric JD_TT:\t%12.7f\n",BJD);
*/
  return(BJD);
}

/*==========================================================================*/

double JD_GUTC(int *date_ymd, int *time_hms){

/***************************************************************************/
/* Calculate UTC Gregorian Julian Day including partial day as decimal     */
/* [Adapted from 'Numerical Recipes, Press et. al (1986)]                  */
/***************************************************************************/

  double jul;
  int ja, jy = date_ymd[0], mm = date_ymd[1], id = date_ymd[2], jm;
  double par_days;


  /* partial JD set by time of day (new JD begins at noon) */
  par_days = (time_hms[0] - 12 + (time_hms[1] + (time_hms[2]/60.0))/60.0)/24.0;


  if (jy == 0)
  {
    printf("There is no year %d.", jy);
    return(-9999);
  }
  if (mm > 2) {
    jm = mm + 1;
  } else {
    --jy;
    jm = mm + 13;
  }

  jul = (long) (floor(365.25*jy)+floor(30.6001*jm)+id+1720995);

  if (id+31L*(mm+12L*date_ymd[0]) >= fIGREG) {
    ja = (long)(0.01*jy);
    jul += 2-ja+(long) (0.25*ja);
  }

  jul += par_days;

  return jul;
}

/*==========================================================================*/

int GUTC_JD(double jul, int *date_ymd){

/***************************************************************************/
/* Calculate UTC Gregorian Julian Day including partial day as decimal     */
/* [From 'Numerical Recipes, Press et. al (1986)]                  */
/***************************************************************************/

  long jalpha,ja,jb,jc,jd,je;
  int iyyy, mm, id;


  if (jul >= tIGREG)
  {
    jalpha = (int)(((jul-1867216)-0.25)/36524.25);
    ja = jul + 1 + jalpha - (int) (0.25*jalpha);
  }
  else if (jul < 0)
  {
    ja = jul + 36525*(1-jul/36525);
  }
  else
    ja = jul;

  jb = ja + 1524;
  jc = (long) (6680.0 + ((double) (jb-2439870)-122.1)/365.25);
  jd = (long) (365 * jc + (0.25*jc));
  je = (long) ((jb-jd)/30.6001);

  id = jb - jd - (long) (30.6001 * je);

  mm = je - 1;
  if (mm > 12)
    mm = mm - 12;

  iyyy = jc - 4715;

  if (mm > 2)
    iyyy -= 1;

  if (iyyy <= 0)
    iyyy -= 1;

  date_ymd[0]=iyyy;
  date_ymd[1]=mm;
  date_ymd[2]=id;

  return 0;
}

/*==========================================================================*/

int leapSeconds(int *ymd){

/***************************************************************************/
/* Number of Leap Seconds to be added                                      */
/***************************************************************************/

  if (ymd[0] >= 2017 && ymd[1] >= 1 && ymd[1] >= 1)       /* Add 37s after 12/31/2016 */
    return(37);
  else if (ymd[0] >= 2015 && ymd[1] >= 7 && ymd[1] >= 1)  /* Add 36s after 06/30/2015 */
    return(36);
  else if (ymd[0] >= 2012 && ymd[1] >= 7 && ymd[1] >= 1)  /* Add 35s after 06/30/2012 */
    return(35);
  else if (ymd[0] >= 2009 && ymd[1] >= 1 && ymd[1] >= 1)  /* Add 34s after 12/31/2008 */
    return(34);
  else
    return(33);    /* Add 33s before 01/01/2009 */
}

/*==========================================================================*/

void Sun2Pos(char *type, int bufflen, int *date_ymd, int *time_hms,
          double *RApos, double *DECpos, double *delta, char *base_dir){

/***************************************************************************/
/***************************************************************************/

  FILE *fptr;
  const char fileprefsuff[] = "tables/ephem/horizons_results_Sun2";
  char *filename=malloc(CHAR_SIZE);
  char buff[bufflen];
  char date[12], time[6];
  char RAhS[3], RAmS[3], RAsS[6];
  char DECdS[4], DECmS[3], DECsS[5];
  char deltaS[17], deldotS[10];
  const char *mhstr[] = {"Jan","Feb","Mar","Apr","May","Jun"
                 ,"Jul","Aug","Sep","Oct","Nov","Dec"};
  char dtstr[12];
  char tmstr[6];
  int yr=date_ymd[0], mh=date_ymd[1], dy=date_ymd[2];
  double RAhms[3];            /* Right Ascension of position in hms (array) */
  double DECdms[3];           /* Declination of position in dms (array) */

  // Produde date and time strings
  sprintf(dtstr, "%d-%s-%02d", yr, mhstr[mh-1], dy);
  sprintf(tmstr, "%02d:%02d", time_hms[0], time_hms[1]);
  sprintf(filename, "%s/%s%s-%d.txt", base_dir, fileprefsuff, type, yr);
/*
  printf("\n%s routine for ", type);
  printf("%s, %s\n",dtstr,tmstr);
  printf("Accessing file: %s\n", filename);
*/
  // Open data file
  fptr = fopen(filename,"r");
  if(fptr == NULL){
    perror("Unable to open file.");
    free(filename);
    exit(1);
  }

  // Find beginning of data lines
  while (fgets(buff,bufflen,fptr)){
      if (strstr(buff,"$S") != 0){
//        printf("\nStart looking through data.\n");
        break;
      }
  }

  fgetc(fptr);   // Skip the $$SOE start of data code

  // Scan through each data line and write data into variables
  for(int i=0;i<(366*36)+1;i++){


    fscanf(fptr, "%s %s     %s %s %s %s %s %s %s   %s\n"
               , date, time
               , RAhS, RAmS, RAsS
               , DECdS, DECmS, DECsS
               , deltaS, deldotS);

    if((strcmp(date,dtstr) == 0) && (strcmp(time,tmstr) == 0)){
/*
      printf("\n%s position from Sun:", type);
      printf("\nDelta:\t%s AU", deltaS);
      printf("\nRA:\t%sh %sm %ss", RAhS, RAmS, RAsS);
      printf("\nDEC:\t%sd %sm %ss", DCdS, DCmS, DCsS);
      printf("\n");
*/
      RAhms[0]=strtod(RAhS,NULL);
      RAhms[1]=strtod(RAmS,NULL);
      RAhms[2]=strtod(RAsS,NULL);
      DECdms[0]=strtod(DECdS,NULL);
      DECdms[1]=strtod(DECmS,NULL);
      DECdms[2]=strtod(DECsS,NULL);

      *RApos  = RAhms2deg(RAhms);
      *DECpos = DECdms2deg(DECdms);

      *delta=strtod(deltaS,NULL);

/*
      printf("\nRA:\t%sh %sm %ss", RAhS, RAmS, RAsS);
      printf("\tRA (deg) = %6.3f\n", *RApos);

      printf("\nDEC:\t%sh %sm %ss", DECdS, DECmS, DECsS);
      printf("\tDEC (deg) = %6.3f\n", *DECpos);

      printf("\ndeltaS:\t%s", deltaS);
      printf("\ndelta:\t%6.3f\n", *delta);
*/

      break;
    }

  }

  fclose(fptr);

  free(filename);

}
