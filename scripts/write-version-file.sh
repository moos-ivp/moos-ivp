echo '#ifndef SVN_REVISION_NUMBER_HEADER' > svn_revision_number.h
echo '   #define SVN_REVISION_NUMBER_HEADER' >> svn_revision_number.h
echo '   #define SVN_REVISION_NUMBER '`svn info . | grep Revision | cut -d ' ' -f2` >> svn_revision_number.h
echo '#endif' >> svn_revision_number.h
