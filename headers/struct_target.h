struct target
{
  char *name;                  /* Pointer to star name string */
  double RAhms[3], RAdeg;      /* Right Ascension of star in hms (array) and degrees; */
  double DECdms[3], DECdeg;    /* Declintion of star in dms (array) and degrees; */
};

typedef struct target TARG;    /* Create structure synonym */
