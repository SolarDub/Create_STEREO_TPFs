#!/bin/bash

# getVal
# Get value assigned to given key name
function getVal() {

  for i in "${!KEYS[@]}"; do            # ! returns array indices of elements
    if [[ "${KEYS[$i]}" = "${name}" ]]  # Check if key element matches with requested key name
    then
      echo ${VALS["${i}"]}              # If so return corresponding values
    fi
  done
}

export -f getVal
