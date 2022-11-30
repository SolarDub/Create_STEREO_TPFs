#!/bin/bash

####################
# Import functions #
####################

SRCDIR="./src/Bash"

source ${SRCDIR}"/prompt.sh"
source ${SRCDIR}"/update.sh"
source ${SRCDIR}"/rewrite.sh"
source ${SRCDIR}"/getVal.sh"
source ${SRCDIR}"/ifHelp.sh"
source ${SRCDIR}"/getCoords.sh"

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


##############################################
# Prompt user for star name if switch exists #
##############################################

if [[ $SWS == *"-s"* ]]
then

  PRMPT1="Enter single star name"  # Create user prompt text
  PRMPT2="(Coordinates will be obtained automatically)"
  PRMPT3="Enter 0 to quit."
  STARENT="$(prompt "$PRMPT1\n$PRMPT2\n$PRMPT3")"  # Prompt user to enter value

  starname="${STARENT%" "}"                    # Remove trailing space

  if [[ $starname == "0" ]]
  then
    exit
  fi

fi

#####################################################
# Prompt user for orbits over which to produce TPFs #
#####################################################

while [[ $ORBS != "10" ]] && [[ $ORBS != "0" ]]; do
  PRMPT1="Enter orbit list separate by spaces (only 10 allowed right now)"  # Create user prompt text
  PRMPT2="(use switch --ohelp to list orbits and dates)"
  PRMPT3="Enter 0 to quit."
  ORBENT="$(prompt "$PRMPT1\n$PRMPT2\n$PRMPT3")"  # Prompt user to enter value
  ORBS="${ORBENT%" "}"                   # Remove trailing space
done

if [[ $ORBS == "0" ]]
then
  exit
fi

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

  starname_NO_WS="$(echo -e "${starname}" | tr -d '[:space:]')"   # Remove remaining whitespaces

  coords="$(getCoords $starname)"

  ./createTPFs.exe $starname_NO_WS $coords $BASDIR $HI1ADIR $OUTDIR $ORBS

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

    ./createTPFs.exe $starname $coords $BASDIR $HI1ADIR $OUTDIR $ORBS

  done < "$STARLISTFILEPATH"

fi
