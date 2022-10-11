#ifndef getStartDate_H
#define getStartDate_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "./headers/constants.h"

int orbitStart(int orbit, int *datetimestartend, char *base_dir);
double JD_GUTC(int *date_ymd, int *time_hms);
int GUTC_JD(double jul, int *date_ymd);

#endif // getStartDate_H
