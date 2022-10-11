#include "./headers/image_date_time.h"

/********************************************************************************************/
/*  Function:                                                                               */
/*    - 'image_date_time'                                                                   */
/*  Inputs:                                                                                 */
/*    - date_ymd:  Pointer to 3-element array containing start of analysis date             */
/*    - time_hms:  Pointer to empty 3-element array for holding first timestamp of day      */
/*    - dt:        HI-1A image cadence (40 minutes)                                         */
/*    - it:        Image timestep counter                                                   */
/*  Output:                                                                                 */
/*    - time_hms:  Pointer to empty 3-element array for holding first timestamp of day      */
/*    - return:    Successful completion, fails if input date is before 03/28/2007          */
/********************************************************************************************/

int image_date_time(int *date_ymd, int *time_hms, int dt, int it)
{


  if (it == 0)
  {

    //  Default first timestamp of day
    time_hms[0]  = 0;
    time_hms[1]  = 9;
    time_hms[2]  = 1;

    //  Reinitialise time mm:ss if beginning before these particular dates/times
    if (date_ymd[0] == 2007)
    {
      if (date_ymd[1] < 3 || (date_ymd[1] == 3 && date_ymd[2] < 28))         /* Before 03/28/2007 */
      {
        printf("Inconsistent data before 03/28/2007. Breaking orbit loop\n\n");
        return -1;
      }
      else if (date_ymd[1] < 5 || (date_ymd[1] == 5 && date_ymd[2] < 24))    /* 03/28/2007 - 05/24/2007 */
      {
        time_hms[1]  = 10;
        time_hms[2]  = 0;
      }
      else if (date_ymd[1] < 10 || (date_ymd[1] == 10 && date_ymd[2] < 23))  /* 05/24/2007 - 10/23/2007 */
      {
        time_hms[1]  = 9;
        time_hms[2]  = 0;
      }
    }
    else if (date_ymd[0] == 2021) //  or after: 2021/02/19,00:00
    {
      if (date_ymd[1] > 2 || (date_ymd[1] == 2 && date_ymd[2] >= 19))
      {
        time_hms[1]  = 8;
        time_hms[2]  = 31;
      }
    }
  }
  else
  {

  /***********************************/
  /* Advance image UTC date and time */
  /***********************************/

    time_hms[1] = time_hms[1] + dt;

    if (time_hms[1] > 59)
    {
      time_hms[0]++;
      time_hms[1]=time_hms[1]-60;

      if (time_hms[0] > 23)
      {
        date_ymd[2]++;
        time_hms[0]=time_hms[0]-24;


        if ((date_ymd[1] == 2 && date_ymd[0]%4 == 0) && (date_ymd[2] > 29))
        {
          date_ymd[1]++;
          date_ymd[2]=date_ymd[2]-29;
        }
        else if ((date_ymd[1] == 2 && date_ymd[0]%4 != 0) && (date_ymd[2] > 28))
        {
          date_ymd[1]++;
          date_ymd[2]=date_ymd[2]-28;
        }
        else if ((date_ymd[1] == 4 || date_ymd[1] == 6 || date_ymd[1] == 9 || date_ymd[1] == 10)
              && (date_ymd[2] > 30))
        {
          date_ymd[1]++;
          date_ymd[2]=date_ymd[2]-30;
        }
        else if (date_ymd[2] > 31)
        {
          date_ymd[1]++;
          date_ymd[2]=date_ymd[2]-31;
        }

        if (date_ymd[1] > 12)
        {
          date_ymd[0]++;
          date_ymd[1]=date_ymd[1]-12;
        }
      }
    }

    //  Switch time mm:ss on these particular dates/times

    // year = 2007
    if (date_ymd[0] == 2007)
    {
      // May 24 2007, 00:10:00
      if (date_ymd[1] == 5 && date_ymd[2] == 24 && time_hms[0] == 0 && time_hms[1] == 10)
      {
        // Set hh:mm:ss to hh:09:00
        time_hms[1]  = 9;
        time_hms[2]  = 0;
      }
      // October 23 2007, 00:09:00
      if (date_ymd[1] == 10 && date_ymd[2] == 23 && time_hms[0] == 0 && time_hms[1] == 9)
      {
        // Set hh:mm:ss to hh:09:01
        time_hms[1]  = 9;
        time_hms[2]  = 1;
      }
    }

    // year = 2021
    else if (date_ymd[0] == 2021)
    {
      // Feb 19 2021, 00:09:01
      if (date_ymd[1] == 2 && date_ymd[2] == 19 && time_hms[0] == 0 && time_hms[1] == 9)
      {
        // Set hh:mm:ss to hh:08:31
        time_hms[1]  = 8;
        time_hms[2]  = 31;
      }
    }
  }

  return 0;
}
