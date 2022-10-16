#include "./headers/getStartDate.h"

/********************************************************************************************/
/*  Function:                                                                               */
/*    - 'getStartDate'                                                                      */
/*  Inputs:                                                                                 */
/*    - orbit:     Orbit number from which to produce image stamp                           */
/*    - RAdeg:     Right Ascension in degrees                                               */
/*    - date_ymd:  Empty 3-element array for holding start of analysis date                 */
/*    - base_dir:  Directory of executable, used for creating filepath to                   */
/*                 orbit start/end time lookup table                                        */
/*  Output:                                                                                 */
/*    - date_ymd:  3-element array containing start of analysis date                        */
/*    - oS_status: Status of access to lookup table                                         */
/*    - JD_status: Status of conversion from JD to UTC                                      */
/********************************************************************************************/

int getStartDate(int orbit, double RAdeg, int *date_ymd, char *base_dir)
{

  int datetimestartend[12];                  /* Array holding orbit start/end dates/times */
  int date_ymd_start[3], time_hms_start[3];  /* Arrays holding orbit start dates, times */

  double RArate = 1.04511631; /* in deg/day (default for years =< 2015) */

  double JDstart;             /* Julian Date of orbit start date */
  double JD;                  /* Julian Date of analysis start date */

  int oS_status, JD_status;

  /* Determine start date of specified orbit */
  oS_status = orbitStart(orbit, datetimestartend, base_dir);

  if (oS_status < 0)
    return oS_status;

  /* Place start date/time into appropriate arrays */
  for (int id = 0; id < 3; id++)
  {
    date_ymd_start[id] = datetimestartend[id];
    time_hms_start[id] = datetimestartend[id+3];
  }

  /* Slightly different orbital rate measured after 2015 */
  if (date_ymd_start[0] > 2015)
  {
    RArate = 1.04383;
  }
//    RArate = 1.04474910;

  /* Convert from UTC start date/time to Julian Date */
  JDstart = JD_GUTC(date_ymd_start, time_hms_start);

//  printf("\nJulian Date in:\t%f\n\n", JDstart);

  /* Add decimal number of days for craft to reach specified object (at image center) from orbit start */
  /* Subtract 12 days to ensure star has not yet reached field of view */
  /* It takes around 19 days for a star to cross the image, so subtracting 15 days gives a safety */
  /* buffer of 2.5 days before the star enters the image, plus just under another day before it */
  /* enters the data collection region. (A 12-day buffer wasn't enough) */
  JD = JDstart + RAdeg/RArate - 15;

  /* Convert from Julian Date to UTC start date/time */
  JD_status = GUTC_JD(JD, date_ymd);

  printf("Orbit %d Start Date:\t%4d %02d %02d\n"\
        , orbit, date_ymd_start[0],  date_ymd_start[1],  date_ymd_start[2]);

  printf("Orbit %d Analysis Date:\t%4d %02d %02d\n\n"\
        , orbit, date_ymd[0],  date_ymd[1],  date_ymd[2]);

  return JD_status;
}

/********************************************************************************************/
/*  Function:                                                                               */
/*    - 'orbitStart'                                                                        */
/*  Inputs:                                                                                 */
/*    - orbit:     Orbit number from which to produce image stamp                           */
/*    - dt:        Empty 12-element array for holding start/end dates/times                 */
/*    - base_dir:  Directory of executable, used for creating filepath to                   */
/*                 orbit start/end time lookup table                                        */
/*  Output:                                                                                 */
/*    - dt:        12-element array containing start/end dates/times                        */
/*    - status:    Status of access to lookup table                                         */
/********************************************************************************************/

int orbitStart(int orbit, int *dt, char *base_dir)
{

  FILE *fptr;            /* File pointer for orbit lookup table */

  const char orbitfilesuff[] = "tables/orbit_0h_cross_times.txt"; /* orbit start/end date/time lookup table */
  char *orbitfilename;   /* Full path and file name for lookup table */

  int oN;                /* Orbit number read from lookup table */
  int status;            /* Used for file open success status */

//  printf("Base_dir: %s\n",base_dir);
//  printf("Orbit file suffix: %s\n",orbitfilesuff);

  orbitfilename = malloc(CHAR_SIZE);       /* Overallocate memory for lookup table filepath and name */
  sprintf(orbitfilename, "%s/%s", base_dir, orbitfilesuff);  /* Produce complete filepath and name */
//  printf("Orbit filename: %s\n",orbitfilename);


  /* Read in orbit number (oN), its start date (dt[0-2]) & time (dt[3-5]) */
  /* and end date (dt[6-8]) and time (dt[9-11]) from lookup table (orbitfilename) */
  /* dt is overwritten for each orbit, so the info returned relates to the orbit specified */

  if ((fptr = fopen(orbitfilename,"r")) == NULL)
  {
    printf("Cannot open file %s.\nExiting.\n",orbitfilename);
    free(orbitfilename);
    status = -1;
    return status;
  }
  else
  {
    for (int io = 0; io < orbit+1; io++)
    {
      status = fscanf(fptr,"%2d %4d %2d %2d %2d %2d %2d %4d %2d %2d %2d %2d %2d"
                     , &oN, &dt[0], &dt[1], &dt[2], &dt[3], &dt[4], &dt[5]
                          , &dt[6], &dt[7], &dt[8], &dt[9], &dt[10], &dt[11]);
    }
  }

  fclose(fptr);

  free(orbitfilename);

  return status;

}
