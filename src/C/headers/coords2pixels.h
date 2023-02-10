#ifndef coords2pixels_H
#define coords2pixels_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <dirent.h>
#include "fitsio.h"
#include "./headers/constants.h"
#include "./headers/struct_target.h"

int regionCheck(char *star, double px, double py);

#endif // coords2pixels
