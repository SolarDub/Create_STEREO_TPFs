#ifndef STEREO_UTC2BJD_H
#define STEREO_UTC2BJD_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <dirent.h>
#include "fitsio.h"
#include "./headers/constants.h"
#include "./headers/struct_target.h"

double JD_GUTC(int *date_ymd, int *time_hms);
int GUTC_JD(double jul, int *date_ymd);
int leapSeconds(int *date_ymd);
void Sun2Pos(char *type, int bufflen, int *date_ymd, int *time_hms,
             double *RApos, double *DECpos, double *delta, char *base_dir);
double RAhms2deg(double *RAhms);
double DECdms2deg(double *DEChms);

#endif // STEREO_UTC2BJD_H
