CMAKE_MINIMUM_REQUIRED(VERSION 2.0)

PROJECT(IVP)

# This is used to supply a preprocessor definition that gets compiled into some
# code, such as lib_marineview.  I really dislike this approach because it's 
# very brittle w.r.t. anything getting moved around, or if you use the code on
# a different system than that on which it was compiled. -CJC
GET_FILENAME_COMPONENT(IVP_DATA_DIR ./data ABSOLUTE)

SUBDIRS(
   docs
   src
)
