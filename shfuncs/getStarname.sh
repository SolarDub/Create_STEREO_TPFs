#!/bin/bash

# getStarname
# Get Starname from user
function getStarname() {

  if [[ $SWS == *"-s"* ]]
  then

    PRMPT1="Enter single star name"  # Create user prompt text
    PRMPT2="(Coordinates will be obtained automatically)"
    PRMPT3="Enter 0 to quit."
    STARENT="$(prompt "$PRMPT1\n$PRMPT2\n$PRMPT3")"  # Prompt user to enter value

    starname="${STARENT%" "}"                    # Remove trailing space

    starname_NO_WS="$(echo -e "${starname}" | tr -d '[:space:]')"   # Remove remaining whitespaces

    echo $starname_NO_WS

    if [[ $starname == "0" ]]
    then
      exit
    fi

  else

    echo ""

  fi
}

export -f getStarname
