

# next few lines are simply to insert icons into
# the console application on windows
IF(WIN32)
    SET(RESOURCES icon1.ico uMS.rc)
ENDIF(WIN32)


IF(APPLE)
# icon files to copy in the bundle
  set( OSX_ICON_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Mac/MOOSV-10-256.icns )
  set( MACOSX_BUNDLE_ICON_FILE "MOOSV-10-256.icns" ) 
  # set where in the bundle to put the icns files
  set_source_files_properties( ${OSX_ICON_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
  # include the icns files in the target
  set( RESOURCES ${RESOURCES} ${OSX_ICON_FILES} )
ENDIF(APPLE)


#build a list of files to compile
SET(UMS_SRCS ScopeGrid.cpp uMS.cpp ScopeTabPane.cpp DBImage.cpp ScopeGrid.h ScopeTabPane.h DBImage.h ${RESOURCES})

FIND_PACKAGE(MOOS 10)

INCLUDE_DIRECTORIES(${fltkvw_DEPEND_INCLUDE_DIRS} ${fltkvw_INCLUDE_DIRS} ${MOOS_INCLUDE_DIRS} ${MOOS_DEPEND_INCLUDE_DIRS})

# Add executable
#ADD_EXECUTABLE( uMS WIN32  MACOSX_BUNDLE ${UMS_SRCS})
ADD_EXECUTABLE( uMS WIN32  ${UMS_SRCS})

# Link the executable
TARGET_LINK_LIBRARIES( uMS ${MOOS_LIBRARIES} ${MOOS_DEPEND_LIBRARIES} ${fltkvw_LIBRARIES} ${fltkvw_DEPEND_LIBRARIES}  )



INSTALL(TARGETS uMS
  BUNDLE DESTINATION /Applications
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
