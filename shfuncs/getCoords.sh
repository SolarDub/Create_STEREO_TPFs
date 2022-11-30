#!/bin/bash

# getVal
# Get value assigned to given key name
function getCoords() {

#
# Run Python script with starname as input argument
# Python outputs coordinates to stderr
# 2>&1 - redirect stderr to stdout
# > /dev/null - ignore all other stdout output
# Assign output to variable
  STRINGIN="$(python3 ./pyfuncs/SIMBAD_getCoords.py "$starname" 2>&1 > /dev/null)"

  REMOVE1=${STRINGIN:1:${#STRINGIN}-2}
  SPLIT=(${REMOVE1//,/ }) # Replace comma with space, then split all using space delimiter

  RAh="${SPLIT[0]#"'"}"
  RAm="${SPLIT[1]}"
  RAs="${SPLIT[2]%"'"}"

  DECd="${SPLIT[3]#"'"}"
  DECm="${SPLIT[4]}"
  DECs="${SPLIT[5]%"'"}"

  coordstr="$RAh $RAm $RAs $DECd $DECm $DECs"

  echo $coordstr

}

export -f getCoords
