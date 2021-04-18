#!/bin/bash 

txtblu=$(tput setaf 4)  # Blue

for folder in *; do
   if [ -d $folder ]; then
      echo -n cd $folder
      cd $folder; 
      if [ -f clean.sh ]; then
	  echo -n "  [Cleaning]" 
	  ./clean.sh
      fi
      echo ""
      cd .. 
   fi
done
