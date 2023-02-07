#!/bin/bash

####################
# Import functions #
####################

#SRCDIR="./src/Bash"
SRCDIR="./shfuncs"

source ${SRCDIR}"/prompt.sh"
source ${SRCDIR}"/update.sh"
source ${SRCDIR}"/rewrite.sh"
source ${SRCDIR}"/getVal.sh"
source ${SRCDIR}"/ifHelp.sh"
source ${SRCDIR}"/getCoords.sh"
source ${SRCDIR}"/checkCoords.sh"
source ${SRCDIR}"/getOrbits.sh"
source ${SRCDIR}"/getStarname.sh"

###################################
# SET UP FILE AND DIRECTORY PATHS #
###################################

SWS=${*}   # Array of switches

# Check if --help switch has been activated
ifHelp

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
  if [[ $SWS == *"-d"* ]] || [[ $SWS == *"-f"* ]]
  then
    # Overwrite read script directory value with path read earlier as BASEDIR
    if [[ $KEY == "THIS_SCRIPT_DIR" ]]
    then
      VAL=$BASEDIR
    fi
    #
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

# Execute if the list of switches is not empty
#
if [[ $SWS == *"-d"* ]] || [[ $SWS == *"-f"* ]]
then
  # Write over directory file names in text filenames
  #
  rewrite $SWS $DIRSFILENAME $KEYS $VALS
fi

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

echo ' '
echo $BASDIR
echo $HI1ADIR
echo $OUTDIR
echo $STARLISTFILEPATH
echo ' '


#############################
# Prompt user for star name #
#############################

starname="$(getStarname)"

#####################################################
# Prompt user for orbits over which to produce TPFs #
#####################################################

ORBS="$(getOrbits)"

echo "Orbits selected: ${ORBS}"
echo ' '

#####################################################################
# Run Target Pixel File creation program for each star in star list #
#####################################################################
# For a given star, retrieve its name and celestial coordinates
# RA: Right Ascension; DEC: Declination
# Name RA_hours RA_minutes RA_seconds DEC_degrees DEC_minutes DEC_seconds
# Each value is separated by spaces, but read (and passed) as a single entity
# The C code splits this single entity into its six constituents and
# passes them into their respective arrays.

if [[ $SWS == *"-s"* ]]
then

  coords="$(getCoords $starname)"
  echo "Coordinates of "${starname}": "${coords}
  echo ""
  check="$(checkCoords $coords)"
  if [[ ${check} == 1 ]]; then
    echo ${starname}" lies outside image bounds. Exiting."
    echo ""
    exit
  fi
  exit
  ./createTPFs.exe $starname $coords $BASDIR $HI1ADIR $OUTDIR $ORBS

else

  # Loop over each star in file, retrieve its name and celestial coordinates
  while read -r info; do

    if [[ $SWS == *"-c"* ]]
    then
      read starname coords <<< "$info"
    else
      read starname <<< "$info"
      coords="$(getCoords $starname)"
    fi

    check="$(checkCoords $coords)"
    if [[ ${check} == 1 ]]; then
      echo ${star}" lies outside image bounds. Moving to next star."
      echo ""
      continue
    fi


    ./createTPFs.exe $starname $coords $BASDIR $HI1ADIR $OUTDIR $ORBS

  done < "$STARLISTFILEPATH"

fi
