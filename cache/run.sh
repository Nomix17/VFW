#!/bin/bash

mediapaths=()
path_mode="fullpath"

while [[ $# -gt 0 ]]; do 
  case "$1" in
    --fullpath | -p) 
      path_mode="fullpath"
      ;;
    --nameonly | -n)
      path_mode="nameonly"
      ;;
    -*)
      echo "Unknown option: $1" 
      exit 1
      ;;
    *)

      if [[ "$path_mode" == "fullpath" ]]; then
        mediapaths+=("$1")
      else
        mediapaths+=("$(pwd)/$1")
      fi
      ;;
  esac
  shift
done

for path in "${mediapaths[@]}"; do
  echo "  $path"
done

cd "/opt/VFW/bin/"
./VFW "${mediapaths[@]}"
