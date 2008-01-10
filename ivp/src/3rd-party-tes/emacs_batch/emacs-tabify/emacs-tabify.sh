# run this in the directory below the .cpp you want to indent

for i in `ls **/*.cpp` `ls *.cpp` `ls **/*.h` `ls *.h` 
do emacs --batch $i -l ~/moos-ivp/ivp/src/3rd-party-tes/emacs_batch/emacs-tabify/emacs-tabify.el -f emacs-format-function
done
