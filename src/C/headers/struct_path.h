struct path
{
  char *base;                  /* Pointer to executable directory string */
  char *HI1A_data;             /* Pointer to base HI-1A image FITS directory string */
  char *stereo_star;           /* Pointer to base stellar directory string */
};

typedef struct path PTH;       /* Create structure synonym */
