#!/usr/bin/python

import sys
import subprocess
import os.path
import math

#===============================================================================

def print_usage():
   print \
"""
Usage: trim_map.py  <infile>  <outfile>  <out-info-file> \\
   -keep  <bottom-lat>  <top-lat>  <left-lon>  <right-lon>  \\
   -size <x-size>  <y-size> \\
   -origin <origin-lat>  <origin-lon>
   
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

<origin-lat> and <origin-lon> give the location of the origin of the (x,y)
coordinate system used in the AUV software.  This script just needs this 
information when writing the .info file.

The script will produce two files: 
   
<outfile> is the name of the .jpg file that this script will produce.  
This script will *not* add a .tif suffix to the filename; you need to specify 
that.  Outfile will be exactly 2048 x 2048 pixels in size.

Note that the image format of the output file is dictacted by the filename
extension you use for <outfile>.  So, for example, if you specify 
"foo.tif", your output file will be a TIFF file.

<out-info-file> describes the details of the output file, suitable for use in 
polyview.  It will presumably be named something like "foo.info".

*** THIS SCRIPT WILL OVERWRITE EXISTING <outfile> AND <out-info-file> FILES! ***
"""

#===============================================================================

nautical_miles_per_degree_lat = 60
meters_per_nautical_mile = 1852

meters_per_degree_lat = nautical_miles_per_degree_lat * meters_per_nautical_mile

#===============================================================================

def get_meters_per_degree_lon(lat_degrees):
   lat_radians = lat_degrees * (2 * math.pi / 360)
   
   # This is approximate, but should be close enough.
   return nautical_miles_per_degree_lat * meters_per_nautical_mile * math.cos(lat_radians)

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

def crop_image(input_filename, output_filename, new_x_pixels, new_y_pixels, crop_x_offset, crop_y_offset):
   cmd = ['convert',  
      '-gravity', 'SouthWest',
      '-crop',  str(new_x_pixels) + 'x' + str(new_y_pixels) \
         + '+' + str(crop_x_offset) + '+' + str(crop_y_offset),
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
   if (len(argv) != 15) \
         or (argv[4] != '-keep') \
         or (argv[9] != '-size') \
         or (argv[12] != '-origin'):
      print_usage();
      sys.exit(1);
      
   try:
      infile         = argv[1]
      outfile        = argv[2]
      out_info_file  = argv[3]
      desired_blat   = float(argv[5])
      desired_tlat   = float(argv[6])
      desired_llon   = float(argv[7])
      desired_rlon   = float(argv[8])
      desired_x_size = int(argv[10])
      desired_y_size = int(argv[11])
      origin_lat     = float(argv[13])
      origin_lon     = float(argv[14])
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
   
   required_output_image_y = int(input_image_y * (desired_lat_span_degrees / input_lat_span_degrees))
   required_output_image_x = int(input_image_x * (desired_lon_span_degrees / input_lon_span_degrees))
   
   if (required_output_image_y > desired_y_size) or (required_output_image_x > desired_x_size):
      sys.exit( \
         "Problem: In order to produce an output image that covers the specified \n" + \
         "   lat/lon range, the output image would need to have width,height = " + \
         str(required_output_image_x) + "," + str(required_output_image_y) + "\n\n" + \
         "   That's bigger than you specified the output image size to be.")

   # Figure out where in the input and output files the origin is.  We really
   # only need to know it for the output file, but just to be helpful we'll 
   # report where it is in the input file.
   input_origin_x_fraction = (origin_lon - input_llon) / input_lon_span_degrees
   input_origin_y_fraction = (origin_lat - input_blat) / input_lat_span_degrees
   
   input_origin_x_pixels = int(input_origin_x_fraction * input_image_x)
   input_origin_y_pixels = int(input_origin_y_fraction * input_image_y)

   # Figure out what pixel offset (both x- and y-) into the original image
   # should be the bottom-left corner of the content that ends up in the
   # output image...
   crop_x_offset = int((desired_blat - input_blat) / input_lat_degrees_per_pixel)
   crop_y_offset = int((desired_llon - input_llon) / input_lon_degrees_per_pixel)
            
   # Figure out the lat/lon bounds of the *actual* output image being produced.
   # This will typically exceed what the user specified with <bottom-lat>, et al
   # because we need to grab extra content from the source image in order for the
   # output image to have the number of pixels specified by the <x-size> and
   # <y-size> command-line args...   
   outfile_llon = input_llon + (crop_x_offset * input_lon_degrees_per_pixel)
   outfile_rlon = outfile_llon + (required_output_image_x * input_lon_degrees_per_pixel)            
   outfile_blat = input_blat + (crop_x_offset * input_lat_degrees_per_pixel)
   outfile_tlat = outfile_blat + (required_output_image_y * input_lon_degrees_per_pixel)
   
   outfile_lon_span_degrees = outfile_rlon - outfile_llon
   outfile_lat_span_degrees = outfile_tlat - outfile_blat
   
   # Figure out where in the output image the origin is, but fractional and 
   # pixel offset...
   outfile_origin_x_fraction = (origin_lon - outfile_llon) / outfile_lon_span_degrees
   outfile_origin_y_fraction = (origin_lat - outfile_blat) / outfile_lat_span_degrees
   
   outfile_origin_x_pixels = int(outfile_origin_x_fraction * required_output_image_x)
   outfile_origin_y_pixels = int(outfile_origin_y_fraction * required_output_image_y)
   
   # Figure out how changes in lat/lon relate to actual distances (using our 
   # simplified projection of everything onto a plane).  We're assuming a small 
   # region, so we can choose an arbitrary latitude within that region for our
   # calculations.
   meters_per_degree_lon = get_meters_per_degree_lon(outfile_tlat)
   avg_meters_per_degree = (meters_per_degree_lon + meters_per_degree_lat) / 2
   
   # What fraction of the output image makes up 100 meters.  Even if our image
   # covered the same number of degrees latitude as it did degrees longitude,
   # this would be dependent on how far from the equator we are.  But if we're
   # really going to boil this down to a single value, not much I can do.  I'll
   # just average it. -CJC
   
   # >> TEMPORARILY COMMENTED OUT UNTIL MIKE B. EXPLAINS FORMULA FOR img_meters. -CJC
   #info_file_img_meters
   
   print "Input image:"
   print "   lat degrees per pixel: %0.10f" % input_lat_degrees_per_pixel
   print "   lon degrees per pixel: %0.10f" % input_lon_degrees_per_pixel
   print "   Origin:"
   print "      fraction into image: x-axis=" + str(input_origin_x_fraction) \
      + ", y-axis=" + str(input_origin_y_fraction)
   print "      pixel offset relative to bottom-left corner: x=" + str(input_origin_x_pixels) \
      + ", y=" + str(input_origin_y_pixels)
   print "\n"
   print "Output image:"
   print "   Size (pixels) needed to contain specified lat/lon region: " + \
      str(required_output_image_x) + " wide, " + str(required_output_image_y) + " high"
   print "   Bottom-left of output image is x=" + str(crop_x_offset) + \
      ", y=" + str(crop_y_offset) + " pixels from bottom-left of input image."
   print "   Lat/lon bounds:"
   print "      lat=[" + str(outfile_blat) + ", " + str(outfile_tlat) + "]"
   print "      lon=[" + str(outfile_llon) + ", " + str(outfile_rlon) + "]"
   print "   Origin:"
   print "      fraction into image: x-axis=" + str(outfile_origin_x_fraction) \
      + ", y-axis=" + str(outfile_origin_y_fraction)
   print "      pixel offset relative to bottom-left corner: x=" + str(outfile_origin_x_pixels) \
      + ", y=" + str(outfile_origin_y_pixels)
   print "   Distances:"
   print "      meters per degree lat: " + str(meters_per_degree_lat)
   print "      meters per degree lon: %0.3f" % (meters_per_degree_lon)
   #print "      .info file's 'img_meters' value will use the average: %0.3f" % avg_meters_per_degree
               
   #skew = abs(meters_per_degree_lon / meters_per_degree_lat)
   #if skew < 0.95:
      #print "      *** WARNING: The ratio (meters per degree lon):(meters per degree lat) is %0.3f.\n" \
         #"          Should your visualization app really act as though it's no big deal?" % (skew)
   print ""            
            
   # Produce the output files...
   crop_image(infile, outfile, desired_x_size, desired_y_size, crop_x_offset, crop_y_offset)
   print "Created image file: " + outfile

   oif = open(out_info_file, "w")
   oif.write("img_centx   = " + str(outfile_origin_x_fraction) + "\n")
   oif.write("img_centy   = " + str(outfile_origin_y_fraction) + "\n")
   oif.write("img_centlat = " + str(origin_lat) + "\n")
   oif.write("img_centlon = " + str(origin_lon) + "\n")
   oif.close()
   print "Created info file: " + out_info_file

#===============================================================================

if __name__ == '__main__':
   main(sys.argv)