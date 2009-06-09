##################################################################
# rough and ready python script to build all latex documentation
# call this by running "python MakeDocumentation"
# NB this is a script to help pnewman maintain the documentation
# and as such is at the moment only runnable on linux
##################################################################

import os , shutil
path = "."
sResultFile = 'buildresult'

def MakePDF(dirname, FileName):
    print "making latex in "+ dirname + "from" +FileName 
    InitialDir = os.getcwd()
    NewDir =  os.path.join(os.getcwd(),dirname)
    os.chdir(NewDir);
   
    if os.path.exists(sResultFile):
        print "removing existing results file..\n"
        os.remove(sResultFile);

    sTee =  ' | tee -a '+ sResultFile;
    sLatexCmd = 'latex ' + FileName + sTee;
    sPDFCmd = 'dvipdf ' + os.path.splitext(FileName)[0] + sTee;
    os.system(sLatexCmd);
    os.system(sLatexCmd);
    os.system(sPDFCmd);
    os.chdir(InitialDir);


def MakeDoxygenDocs(SourcePath):


    DoxyFile = 'Template.doxy';
    TmpDoxy = 'MOOS.doxy'
    ProjectName = os.path.basename(SourcePath);
    Destination = SourcePath.replace(os.pardir,os.path.join(os.pardir,'Docs','Source'));

    print 'Making Doxygen documentation for %s in %s ' % (SourcePath,Destination);
    
    file = open(DoxyFile, "r") #Opens the file in read-mode
    text = file.read() #Reads the file and assigns the value to a variable
    file.close() #Closes the file (read session)
    
    text = text.replace('$PROJECTNAME',ProjectName);
    text = text.replace('$DOCDIR',Destination);
    text = text.replace('$SOURCEDIR',SourcePath);
       
    file = open(TmpDoxy, "w") #Opens the file again, this time in write-mode
    file.write(text)
    file.close() #Closes the file (write session)
    
    sDoxygenCommand = 'doxygen %s' % TmpDoxy;
    # finally call doxygen..
    os.system(sDoxygenCommand);

    #need to copy header image file to html directory..
    shutil.copy('resources/moose7.gif',os.path.join(Destination,'html'));

    #make latex...
    here = os.getcwd();
    os.chdir(os.path.join(Destination,'latex'));
    os.system('make');
    os.system('dvipdf refman.dvi %s.pdf' % (ProjectName)); 
    os.chdir(here);

    
#make latex on each of the directories
for directory in os.walk(path):
    if directory[0].endswith("latex") and  directory[0]!='Legacy' and directory[2].count('doxygen.sty')==0:
       for File in directory[2]:       
           if File.endswith(".tex"):
              MakePDF(directory[0],File)



#make doxygen files
sourcepath = '../';
DoxygenDirs = ["MOOSLIB", "MOOSGenLib"];
for directory in os.walk(sourcepath):

    bn = os.path.basename(directory[0]);
    parts = directory[0].split(os.sep);
        
    if(bn in DoxygenDirs and len(parts)>1 and parts[1] != 'Docs' ):
        MakeDoxygenDocs(directory[0]);
    


# print a listing of the outcomes
print "Any Errors in compilation are stated here..."
os.system('find -name '+ sResultFile + ' -print | xargs grep Error');
print "Any warnings in compilation are stated here..."
os.system('find -name '+ sResultFile + ' -print | xargs grep Warning');

