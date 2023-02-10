#!/bin/bash

# getOrbits
# Get list of orbit numbers from user (Fixed at 10 for now)
function getOrbits() {

  while [[ $ORBS != "10" ]] && [[ $ORBS != "0" ]]; do
    PRMPT1="Enter orbit list separate by spaces (only 10 allowed right now)"  # Create user prompt text
    PRMPT2="(use switch --ohelp to list orbits and dates)"
    PRMPT3="Enter 0 to quit."
    ORBENT="$(prompt "$PRMPT1\n$PRMPT2\n$PRMPT3")"  # Prompt user to enter value
    ORBS="${ORBENT%" "}"                   # Remove trailing space
  done

  echo $ORBS

}

export -f getOrbits
