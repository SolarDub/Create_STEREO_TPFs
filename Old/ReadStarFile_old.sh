#!/bin/bash

####################
# Import functions #
####################

source "./shfuncs/prompt.sh"
source "./shfuncs/update.sh"
source "./shfuncs/getVal.sh"

###################################
# SET UP FILE AND DIRECTORY PATHS #
###################################

SWS=${*}   # Array of switches

#
# Absolute path this script is in
#
BASEDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

DIRSFILENM="directory_file_lists.txt"
DIRSFILENAME=${BASEDIR}/"tables"/${DIRSFILENM}

#
# Initialize empty key/value arrays
#
KEYS=()  # Define empty key (input value description) array
VALS=()  # Define empty value (input file/directory) array

#
# Read each key/value pair line from list file
# separated with '='
#
while IFS='=', read -r KEY VAL; do    

  # Execute if the list of input switches is not empty
  #
  if [ -n "$SWS" ]    
  then
    # Update directories/filenames as signified by switches
    #
    OUTVAL="$(update $SWS $KEY $VAL)"
  else
    OUTVAL="${VAL%"/"}"   # Remove trailing "/"
  fi

  # Add keys/values to respective arrays
  #
  KEYS+=($KEY)
  VALS+=($OUTVAL)

done < "$DIRSFILENAME"

# Number of defined directories
#
NUM_VALS=${#VALS[@]}    

# Execute if the list of switches is not empty
#
if [ -n "$SWS" ]    
then
  rm "$DIRSFILENAME"    # Remove the directory/file text file ready to be rewritten

# Write directory and filenames
  for (( I=0; I<${NUM_VALS}; I++)); do
    echo "${KEYS["${I}"]}=${VALS["${I}"]}" >> "$DIRSFILENAME"
  done
fi

#####################################################################
# Run Target Pixel File creation program for each star in star list #
#####################################################################

# Base path to this script, programs and executables

name='THIS_SCRIPT_DIR'
BASDIR="$(getVal $KEYS $name)"

# Base path to STEREO HI-1A image data

name='HI1A_DATA_DIR'
HI1ADIR="$(getVal $KEYS $name)"

# Outpath for Target Pixel File fits file

name='FITS_OUT_DIR'
OUTDIR="$(getVal $KEYS $name)"

# Path to file containing list of stars and their coordindates

name='STAR_COORD_LIST_FILE'
STARLISTFILE="$(getVal $KEYS $name)"

STARLISTFILEPATH=${BASEDIR}/"starLists"/${STARLISTFILE}

echo $BASDIR
echo $HI1ADIR
echo $OUTDIR
echo $STARLISTFILEPATH

# Loop over each star in file, retrieving its name and celestial coordinates
# RA: Right Ascension; DEC: Declination
# Name RA_hours RA_minutes RA_seconds DEC_degrees DEC_minutes DEC_seconds
# Each value is spearated by spaces

while read -r info; do

  read NAME RAh RAm RAs DECd DECm DECs <<< "$info"

  ./createTPFs.exe $NAME $RAh $RAm $RAs $DECd $DECm $DECs $BASDIR $HI1ADIR $OUTDIR

done < "$STARLISTFILEPATH"
