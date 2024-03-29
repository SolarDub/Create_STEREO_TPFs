#!/bin/bash

# ifHelp
# Prints help file to screen if switch is selected
function ifHelp() {

  if [[ $SWS == *"-h"* ]] || [[ $SWS == "" ]]
  then
    cat ./help/help.txt
    exit
  fi

  if [[ $SWS == *"--ohelp"* ]]
  then
    echo ' '
    echo 'Or Start Date/Time     End Date / Time'
    cat ./tables/orbit_0h_cross_times.txt
    echo ' '
    exit
  fi

  if [[ $SWS == *"--dhelp"* ]]
  then
    echo ' '
    echo 'KEYNAME=value'
    cat ./tables/directory_file_lists.txt
    echo ' '
    exit
  fi

}

export -f ifHelp
