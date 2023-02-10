#include "./headers/createFilename.h"

/********************************************************************************************/
/*  Function:                                                                               */
/*    - 'createFilename'                                                                    */
/*  Inputs:                                                                                 */
/*    - date_ymd:      Pointer to 3-element array containing date (y, m, d)                 */
/*    - time_hms:      Pointer to 3-element array containing time (h, m, s)                 */
/*    - HI1A_base_dir: Pointer to string filepath to base STEREO HI-1A data directory       */
/*  Output:                                                                                 */
/*    - file_name:     Pointer to string filepath/filename of current HI-1A image FITS file */
/*    - s_datetime:    String (character array) containing date_time                        */
/********************************************************************************************/

/*==========================================================================*/

void createFilename(int it, int *date_ymd, int *time_hms, char *HI1A_base_dir, char *file_name, char s_datetime[])
{

/***************************************************************************/
/* Creates character string combining directory path and fits filename     */
/***************************************************************************/

/*
  FILE variable initialization */
  char s_day[3], s_month[3], s_year[5], s_date[12];
  char s_hr[3], s_min[3], s_sec[3], s_time[10];
  char *dir_name=malloc(CHAR_SIZE);

  // Create year, month, day and date strings
  sprintf(s_year, "%04d", date_ymd[0]);
  sprintf(s_month, "%02d", date_ymd[1]);
  sprintf(s_day, "%02d", date_ymd[2]);
  sprintf(s_date, "%s%s%s",s_year,s_month,s_day);

  // Create time string
  sprintf(s_hr, "%02d", time_hms[0]);
  sprintf(s_min, "%02d", time_hms[1]);
  sprintf(s_sec, "%02d", time_hms[2]);
  sprintf(s_time, "%s%s%s",s_hr,s_min,s_sec);

  // Create date_time string
  sprintf(s_datetime, "%s_%s", s_date, s_time);

  // Create path to image FITS file
  sprintf(dir_name,"%s/%s/%s/%s",HI1A_base_dir,s_year,s_month,s_day);

  // Attach path to created image FITS file name
  sprintf(file_name,"%s/%s_%s_24h1A_br01.fits",dir_name,s_date,s_time);

  free(dir_name);

  printf("\n%d File: %s",it,file_name);

  // Check whether file exists and report result
  if( access( file_name, F_OK) == 0)
  {
    printf("\nFile exists, ");
  }
  else
  {
    printf("\nFile doesn't exist.\n");
  }


}
