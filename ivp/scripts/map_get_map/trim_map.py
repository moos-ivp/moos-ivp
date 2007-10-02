#!/usr/bin/python

import sys
import subprocess
import os.path

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
   cmd = ['identify',  '-format', '%w %h', filename]
   
   p = subprocess.Popen(args=cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
   (p_stdout, p_stderr) = p.communicate()
   
   if p.returncode != 0:
      print >> sys.stderr, "Command '" + ' '.join(cmd) + "' failed.  Here's it's output:\n"
      sys.exit(p_stderr)
      
   results = p_stdout.strip().split(' ')
   assert(len(results) == 2)
   return int(results[0]), int(results[1])

#===============================================================================

def crop_image(input_filename, output_filename, new_x_pixels, new_y_pixels):
   cmd = ['convert',  
      '-gravity', 'SouthWest',
      '-crop',  str(new_x_pixels) + 'x' + str(new_y_pixels) + '+0+0',
      input_filename, 
      output_filename]
   
   p = subprocess.Popen(args=cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
   (p_stdout, p_stderr) = p.communicate()
   
   if p.returncode != 0:
      print >> sys.stderr, "Command '" + ' '.join(cmd) + "' failed.  Here's it's output:\n"
      sys.exit(p_stderr)
      
   results = p_stdout.strip().split(' ')

#===============================================================================

def parse_input_img_filename(filename):
   f = os.path.splitext(os.path.basename(filename))[0]
   fname_parts = f.split('_')
   
   if (len(fname_parts) != 8) \
         or (fname_parts[0] != 'blat') \
         or (fname_parts[2] != 'tlat') \
         or (fname_parts[4] != 'llon') \
         or (fname_parts[6] != 'rlon'):
      sys.exit("The input file's name doesn't have the required structure.")
      
   blat = float(fname_parts[1])
   tlat = float(fname_parts[3])
   llon = float(fname_parts[5])
   rlon = float(fname_parts[7])
   
   return (blat, tlat, llon, rlon)

#===============================================================================

def main(argv):
   if '-keep' in argv:
      if len(argv) != 10:
         print_usage();
         sys.exit(1);
         
      try:
         infile         = argv[1]
         desired_blat   = float(argv[3])
         desired_tlat   = float(argv[4])
         desired_llon   = float(argv[5])
         desired_rlon   = float(argv[6])
         desired_x_size = int(argv[7])
         desired_y_size = int(argv[8])
         outfile        = argv[9]
      except:
         print sys.exc_info()
         print "\n\n"
         print_usage()
         sys.exit(1)
      
      # Confirm that the user isn't asking to grow the image...
      (input_image_x, input_image_y) = get_image_xy_size(infile)
      if (desired_x_size > input_image_x) or (desired_y_size > input_image_y):
         sys.exit("You specified an output image of width, height=" + \
            str(desired_x_size) + ", " + str(desired_y_size) + "\n" + \
            "But the input image has width, height of " + \
            str(input_image_x) + ", " + str(input_image_y) + "\n" + \
            "The output image cannot be bigger on either axis than the input image.")
      
      # Confirm that the desired image actually lies within the input image.
      # Note that this probably only works for images that don't span the poles,
      # prime meridian, etc.
      #
      # We also assume that the image is oriented with the increasing-y axis 
      # pointing north.  This assures us that the bottom latitude is < the top
      # latitude (as long as the image doesn't span a pole of the earth), and 
      # the left longitude is less than the right longitude (as long as the 
      # image doesn't cross the prime meridian)...
      (input_blat, input_tlat, input_llon, input_rlon) = parse_input_img_filename(infile)
      
      if ((desired_blat < input_blat) or (desired_blat > input_tlat)):
         sys.exit("Problem: <bottom-lat> is outside the latitude range of the input file.")
      
      if ((desired_tlat < input_blat) or (desired_tlat > input_tlat)):
         sys.exit("Problem: <top-lat> is outside the latitude range of the input file.")
         
      if (desired_blat > desired_tlat):
         sys.exit("Problem: <bottom-lat> is greater than <top-lat>.")
            
      
      if ((desired_llon < input_llon) or (desired_llon > input_rlon)):
         sys.exit("Problem: <left-lon> is outside the longitude range of the input file.")
      
      if ((desired_rlon < input_llon) or (desired_rlon > input_rlon)):
         sys.exit("Problem: <right-lon> is outside the longitude range of the input file.")
         
      if (desired_llon > desired_rlon):
         sys.exit("Problem: <left-lon> is greater than <right-lon>.")
         
      # Figure out the relationship between pixels and lat/lon.
      #
      # This is only an approximation, because lines of longitude aren't 
      # parallel.  But it's safe enough at the scales we work at...
      input_lat_span_degrees = input_tlat - input_blat
      input_lon_span_degrees = input_rlon - input_llon
      
      desired_lat_span_degrees = desired_tlat - desired_blat
      desired_lon_span_degrees = desired_tlat - desired_blat
      
      input_lat_degrees_per_pixel = input_lat_span_degrees / input_image_y
      input_lon_degrees_per_pixel = input_lon_span_degrees / input_image_x
      
      print "Input image:"
      print "   lat degrees per pixel: " + str(input_lat_degrees_per_pixel)
      print "   lon degrees per pixel: " + str(input_lon_degrees_per_pixel)
      
      required_output_image_y = int(input_image_y * (desired_lat_span_degrees / input_lat_span_degrees))
      required_output_image_x = int(input_image_x * (desired_lon_span_degrees / input_lon_span_degrees))
      
      if (required_output_image_y > desired_y_size) or (required_output_image_x > desired_x_size):
         sys.exit( \
            "Problem: In order to produce an output image that covers the specified \n" + \
            "   lat/lon range, the output image would need to have width,height = " + \
            str(required_output_image_x) + "," + str(required_output_image_y) + "\n\n" + \
            "   That's bigger than you specified the output image size to be.")
             
      crop_image(infile, outfile, desired_x_size, desired_y_size)
      
      
   else:
      #TODO
      pass

#===============================================================================

if __name__ == '__main__':
   main(sys.argv)