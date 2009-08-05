find . -name "*.cpp" -or -name "*.h" -or -name CMakeLists.txt | grep -v "lib_fltk-1.1.7/" | xargs dos2unix
