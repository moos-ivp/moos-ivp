%==================================================
% File: plotx.m
% Date: Dec 16th, 2021
% Auth: M. Benjamin
% Info: Plotting data read in from a file using 
%       args provided from the system command line
%==================================================
%  $ matlab plotx.m -nodisplay -nosplash \
%    -r "plotx(\"data.log\", \"image.png\")"

function plotx(arg1,arg2)
clf
A=load(arg1);

plot(A(:,1),A(:,2   ), 'Marker', 's', 'MarkerSize', 16, 'MarkerEdgeColor', 'red', ...
    'MarkerFaceColor', [0.7 0.4 0.4]);

saveas(gcf,arg2)
exit
end
