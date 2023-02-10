#ifndef createTPFs_H
#define createTPFs_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include "fitsio.h"
#include "./headers/constants.h"
#include "./headers/struct_target.h"
#include "./headers/struct_path.h"

void distArgs(int argc, char *argv[], TARG *ptr_star, PTH *ptr_dir, int *orbits, int Norbs);
double RAhms2deg(double *RAhms);
double DECdms2deg(double *DEChms);
int getStartDate(int orbit, double RAdeg, int *date_ymd, char *base_dir);
void createFilename(int it, int *date_ymd, int *time_hms, char *HI1A_base_dir, char *file_name, char s_datetime[]);
int image_date_time(int *date_ymd, int *time_hms, int dt, int it);
void acquireImage(fitsfile *infptr, float *image);
int coords2pixels(fitsfile *fptr, TARG *ptr_star, double *pxvl, double *pyvl);
double STEREO_UTC2BJD(int *date_ymd, int *time_hms, TARG *ptr_star, char *base_dir);
void produceStamp(float *image, int, int, float *stamp);
void output2fits(int TL , double *bjd,
                 float *s, int N_good_img, double *p1, double *p2, char dtstr[][DTS_SIZE],
                 TARG *ptr_star, PTH *ptr_dir, int orbit);

#endif // createTPFs_H
