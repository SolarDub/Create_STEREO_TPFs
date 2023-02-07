#!/bin/bash

# checkCoords
# Check that the ecliptic latitude of the star lies within the image bounds
function checkCoords() {

#
# Run Python script with starname as input argument
# Python outputs coordinates to stderr
# 2>&1 - redirect stderr to stdout
# > /dev/null - ignore all other stdout output
# Assign output to variable
  STRINGIN="$(python3 ./pyfuncs/celestial2ecliptic.py "$coords" 2>&1 > /dev/null)"

  VAL=${STRINGIN:1:1}

  echo $VAL   # Return coordinate check flag: 0(1) in(out)side image bounds

}

export -f checkCoords
