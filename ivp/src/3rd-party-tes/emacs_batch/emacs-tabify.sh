# run this in the directory below the .cpp you want to indent

for i in `ls **/*.cpp` `ls *.cpp` `ls **/*.h` `ls *.h` 
do emacs --batch $i -l ~/emacstest/emacs-tabify -f emacs-format-function
done
