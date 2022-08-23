
function make_imatlab
%this is a simple build script for iMatlab. Simply run make_imatlab from 
%the matlab command line and follow instructions. Basically you get given
%the choice to autodiscover the location of the MOOS source tree and
%the build you want to link against or specify them yourself.


%ask the user if they want to discover the installation of MOOS or specitfy
%it manually
autodiscover = yesno('Do you want to autodiscover an appropriate MOOS installation?');

if(~autodiscover)

    fprintf(['\n\n\nSTEPS\n********\n'...
    '\n\n1) You need to specify in the make_imatlab.m file the root of the libMOOS source tree.\n' ...
    '   This path should end in Core/libMOOS and contain App, Comms and Utils directories. \n' ...
    '2) You need ot specify in the same file the location (directory) of the built MOOS\n'...
    '   library libMOOS.\n'...
    '\nThe lines you need to edit are near the information printed below\n']);

    edit_location=dbstack('-completenames')
    
    %these are some defaults that will never work - YOU NEED TO EDIT THESE
    include_root = '/Users/pnewman/code/CompactMOOSProjects/CoreMOOS/Core/libMOOS';
    library_path = '/Users/pnewman/code/CompactMOOSProjects/CMB/lib';    
    
else
    if(ismac || isunix)
        [status,stdio] = system('cmake ./build_help');
        if(status~=0)
            disp(stdio);
            error('CMake invocation failed')
        end;
        stdio = regexp(stdio, '[\f\n\r]', 'split');
        for i = 1:length(stdio)
            if(length(stdio{i})~=0)            
                %is this an include path
                t = textscan(stdio{i}, 'INCLUDE:%s'); 
                theline = char(t{1});
                if(~isempty(theline))
                    tok = 'libMOOS/include';
                    j = strfind(theline,tok);
                    if(~isempty(j))                   
                        [include_root, last_part, ext] = fileparts(theline);
                    end
                end;

                %is is library path
                t = textscan(stdio{i}, 'LIB:%s');                        
                theline = char(t{1});
                if(~isempty(theline))
                    [library_path, library_name, ext] = fileparts(theline) ;
                end;
            end;
        end;


    end;

    fprintf('   found includes at %s\n',include_root)
    fprintf('   found library  at %s\n',library_path)
    fprintf('   library name is   %s(%s)\n',library_name,ext)

    reply = input('\ndo these look right? [Y/n]\n', 's');
    if(isempty(reply))
        reply = 'y';
    end
    if(lower(reply)~='y')
        return 
    end
end




components = {'App','Comms', 'Utils'};
include_directive = [' -I',include_root,'/include'];
for(i = 1:length(components))
    new_path = [' -I',include_root,'/',components{i},'/include'];
    include_directive= [include_directive, new_path];
    fprintf(' adding directive %s\n',new_path)
end;


cmd = ['mex ', include_directive,  ' -L', library_path, ' -lMOOS',' iMatlab.cpp mexHelpers.cpp'];

fprintf('\nAbout to invoke mex compiler with:\n %s \n\n',cmd);

if(~yesno('Does this look right?'))
    return;
end
    

fprintf('compiling.....\n')
eval(cmd)
fprintf('.....done\n')





function ok = yesno(prompt)
reply = input(['\n' prompt '[Y/n]\n'],'s');
if(isempty(reply))
    reply = 'y';
end
if(lower(reply)~='y')
    ok = 0;
else
    ok = 1;
end


