#!/usr/bin/python

import sys
import subprocess

#===============================================================================

def print_usage():
   print \
"""
Usage: trim_map.py  <infile>  [ -keep <bottom-lat> <top-lat>  <left-lon>  <right-lon> ] <x-size>  <y-size>  <outfile>
   
If the the '-keep' option and associated parameters are supplied, this trims the
input image in a way that keeps the specified region.  If '-keep' and its 
parameters are not specified, then the image is simply cropped down to the
specified output size, removing the top and right parts of the image as needed.
   
<infile>  must have a name of the form produced by the 'map_get_map.pl' script
so that this script can figure out its lat/lon boundaries.   

<bottom-lat> ... <right-lon> describe the region within the input file that must 
be present in the output file.  (If this script is unable to meet this 
requirement, the script will fail.)

<x-size> and <y-size> give the size of the output image, in pixels.  If the
input image is smaller in either dimension than the output image is specified to
be, this script will fail.

The script will produce two files: 
   
<outfile> is the name of the .jpg file that this script will produce.  
This script will *not* add a .jpg suffix to the filename; you need to specify 
that.  Outfile will be exactly 2048 x 2048 pixels in size.

<outfile.info> describes the details of the output file, suitable for use in 
pViewer.
"""

#===============================================================================

def get_image_xy_size(filename):
   """
   Returns a tuple (x, y) giving the size in pixels of the specified image file.
   If some problem is encountered, this prints an error and exits.
   """
   cmd = ['identify',  '-xformat', '%w %h', filename]
   
   p = subprocess.Popen(args=cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
   (p_stdout, p_stderr) = p.communicate()
   
   if p.returncode != 0:
      print >> sys.stderr, "Command '" + cmd + "' failed.  Here's it's output:\n\n"
      sys.exit(p_stderr)
      
   results = p_stdout.strip().split(' ')
   assert(len(results) == 2)
   return int(results[0]), int(results[1])

#===============================================================================

def main(argv):
   if '-keep' in argv:
      if len(argv) != 10:
         print_usage();
         sys.exit(1);
         
      try:
         infile         = argv[1]
         bottom_lat     = float(argv[3])
         top_lat        = float(argv[4])
         left_lon       = float(argv[5])
         right_lon      = float(argv[6])
         desired_x_size = int(argv[7])
         desired_y_size = int(argv[8])
         outfile        = argv[9]
      except:
         print sys.exc_info()
         print "\n\n"
         print_usage()
         sys.exit(1)
      
      print get_image_xy_size(infile)
      
   else:
      #TODO
      pass

#===============================================================================

if __name__ == '__main__':
   main(sys.argv)