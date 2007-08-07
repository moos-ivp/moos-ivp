PROJECT(IVP_ARTFIELD_GENERATOR)

SET(SRC
   artfieldgeneratorMain.cpp
   )

ADD_EXECUTABLE(artfieldgenerator ${SRC})

TARGET_LINK_LIBRARIES(artfieldgenerator 
   MOOS 
   MOOSGen
   mbutil
   )
