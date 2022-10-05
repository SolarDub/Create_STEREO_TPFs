#!/bin/bash

# update
# If "-d"/"-u" switch is included, rewrite directory/file text file
function rewrite() {

  if [[ $SWS == *"-d"*  ]] || [[ $SWS == *"-f"*  ]]
  then
    # Number of defined directories
    #
    NUM_VALS=${#VALS[@]}

    rm "$DIRSFILENAME"    # Remove the directory/file text file ready to be rewritten

  # Write directory and filenames
    for (( I=0; I<${NUM_VALS}; I++)); do
      echo "${KEYS["${I}"]}=${VALS["${I}"]}" >> "$DIRSFILENAME"
    done
  fi
}

export -f rewrite
