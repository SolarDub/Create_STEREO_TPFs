#!/bin/bash

# getCoords
# Get star coordinates using Python script that performs a Simbad request
function getCoords() {

#
# Run Python script with starname as input argument
# Python outputs coordinates to stderr
# 2>&1 - redirect stderr to stdout
# > /dev/null - ignore all other stdout output
# Assign output to variable
  STRINGIN="$(python3 ./src/Python/SIMBAD_getCoords.py "$star" 2>&1 > /dev/null)"

  REMOVE1=${STRINGIN:1:${#STRINGIN}-2}
  SPLIT=(${REMOVE1//,/ }) # Replace comma with space, then split all using space delimiter

  RAh="${SPLIT[0]#"'"}"
  RAm="${SPLIT[1]}"
  RAs="${SPLIT[2]%"'"}"

  DECd="${SPLIT[3]#"'"}"
  DECm="${SPLIT[4]}"
  DECs="${SPLIT[5]%"'"}"

  coordstr="$RAh $RAm $RAs $DECd $DECm $DECs" # Combine coordinates as a string with space delimiters

  echo $coordstr   # Return coordinates

}

export -f getCoords
