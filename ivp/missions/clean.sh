#!/bin/bash 

txtrst=$(tput setaf 0)  # Reset
txtred=$(tput setaf 1)  # Red
txtgrn=$(tput setaf 2)  # Green
txtylw=$(tput setaf 3)  # Yellow
txtblu=$(tput setaf 4)  # Blue
txtpur=$(tput setaf 5)  # Purple
txtcyn=$(tput setaf 6)  # Cyan
txtwht=$(tput setaf 7)  # White

for file in *; do
   if [ -d $file ]; then
      cd $file; 
      if [ -e clean.sh ]; then
	  echo Cleaning directory $file
	  ./clean.sh
      else
	  echo ${txtred} Directory $file has no clean.sh script ${txtrst}
	  tput setaf 0
      fi
      cd .. 
   fi
done
