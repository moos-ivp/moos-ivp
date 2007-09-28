#!/usr/bin/perl

#
#
# Olivier Koch, koch@csail.mit.edu
# 18 Feb 2007
#
# Grab satellite images from Google Maps at maximum zoom level
# and store them with top left and bottom right lat,lon values
# in the name.
#
#
#


use POSIX;  # to get ceil() and floor()

print "\nMap Image Generator\n(images from Google)\n\n";
#print " Top left lat-lon (like 38.872105 -77.202770)  --> "; $topleft_latlon = <>;
#print " Bottom right lat-lon --> "; $bottomright_latlon = <>;

# Sample RNDF
#$topleft_latlon = "38.877049 -77.208471";
#$bottomright_latlon = "38.864897 -77.197511";
#$dataset_dir="Sample_RNDF"

# Weymouth
# $topleft_latlon = "42.158754 -70.949703";
# $bottomright_latlon = "42.139587 -70.924934";
# $dataset_dir="Weymouth"

# Dabob Bay - Washington State
$topleft_latlon = "47.704800 -122.936200";
$bottomright_latlon = "47.654800 -122.886200";
$dataset_dir="Dabob_Bay";

# East Cambridge
#$topleft_latlon = "42.375455 -71.091049";
#$bottomright_latlon = "42.362969 -71.076752";
#$dataset_dir="East_Cambridge"

# Cambridge Port
#$topleft_latlon = "42.356225 -71.106077";
#$bottomright_latlon = "42.355704 -71.103537";
#$dataset_dir="Cambridge_Port"

# satellite zoom ranges from 2 (low res) to 19 (high res)
# map zoom ranges from 17 (low res) to 0 (high res)

# This leads to name collisions...
#$sat_zoom = 14;

$sat_zoom = 14;
$map_zoom = 17-$sat_zoom;

($min_lat,$max_lat,$min_lon,$max_lon) = parse_input( $topleft_latlon, $bottomright_latlon );

print "Data set: $dataset_dir\n";
print "lat: [$min_lat, $max_lat]\n";
print "lon: [$min_lon, $max_lon]\n";

$delta_deg = 360.0 / 2**$sat_zoom;
$delta_deg /= 2.0; # to make sure that we get all images

$dt = 1.0 / 2**$sat_zoom;

#print "delta_deg: $delta_deg\n";

$nlat = floor(($max_lat - $min_lat)/$delta_deg) + 1;
$nlon = floor(($max_lon - $min_lon)/$delta_deg) + 1;
$ntiles = $nlat * $nlon;

print "Number of image tiles: $nlat x $nlon = $ntiles\n";

$cmd_append = "";

$row_count = 0;

# Ensure the dataset's subdirectory exists
qx\mkdir -p $dataset_dir\;

for ($la = $min_lat;$la <= $max_lat;$la += $delta_deg) {

    $cmd_append_row = "convert ";

    $new_image = 0;

    # For the current row, acquire the of sub-images that should comprise a row
    # in the final output image.  Also build up the cmd-line argument list for
    # a call to the 'convert' program, which will combine the sub-images of this
    # row to produce a single image for the entire row...
    for ($lo = $min_lon; $lo < $max_lon; $lo += $delta_deg) {
#         print "*** ROW_COUNT: $row_count\n";
	
# 	print "lat-lon: $la $lo\n";

        $subimage_filename = $dataset_dir . "/" . &compute_subimage_filename( $la, $lo, $sat_zoom );
my $z =&compute_subimage_filename( $la, $lo );
print "$la  $lo  $z\n";

$z = &latlon2qrts($la, $lo, $sat_zoom);
print "+++ qrts=$z\n";



#if ($z eq "47.656137.122.890319.47.654287.122.887573.jpg")
#{
#print "*** la=$la lo=$lo row_count=$row_count filename=$z\n";
#}

        print "CCC: subimage_filename = $subimage_filename\n";
   
        if ( ! -e $subimage_filename ) {
           $new_image = 1;
            
           $qrts_string = &latlon2qrts($la, $lo);
           $status = &get_google_aerial_image($qrts_string, $subimage_filename);
           if ($status == "error-404") {
               &get_google_map_tile( $la, $lo, $map_zoom, $subimage_filename );
           }
           elsif ($status == "forbidden") {
              print "You have been blacklisted by Google.  You need to wait a while and re-try.\n";
              exit;
           }
        }

 	$cmd_append_row .= "$subimage_filename ";
    }
    
    # Build a new row-image if we introduced a new sub-image file for this row,
    # or if the row image doesn't already exist...
    $row_filename = "$dataset_dir/tile-row$row_count.jpg";
    if ( $new_image or ( ! -e $row_filename ) ) {
       $cmd_append_row .= "+append $row_filename";
   
       qx\$cmd_append_row\;
   
       print "BBB: Row Command: $cmd_append_row\n";
   
       $cmd_append = "$row_filename $cmd_append";
    }

    $row_count = $row_count + 1;
}

# Combine the row-images to produce the final output image...
$cmd_append = "convert $cmd_append -append output.jpg";

qx\$cmd_append\;

print "$cmd_append\n";

exit;

#===============================================================================

sub xy2latlon {
    my ($x,$y) = @_;
    my $PI = 3.1415926535897;

    my $llon = $x * 360.0 - 180.0;
    $q = exp(4*$PI*($y-.5));
    $a = asin(($q-1)/($q+1));
    my $llat = -180.0*$a/$PI;
    return ($llat,$llon);
}

#===============================================================================

sub toNormalizedCoord {
    my ($lat,$lon) = @_;
    
    if ( $lon > 180 ) {
	$lon = $lon - 360;
    }

    $lon = $lon / 360;
    $lon = $lon + 0.5;

    $lat = 0.5 - ((log( tan( $PI/4 + (( 0.5 * $PI * $lat ) / 180 ))) / $PI) / 2.0);
    return ($lat,$lon);
}

#===============================================================================

sub parse_input {
    my ($tl,$br) = @_;
    
    @c = split( /[\t ]/, $tl);
    #print("1: $c[0], 2: $c[1]\n");
    my $mmax_lat = $c[0];
    my $mmin_lon = $c[1];
    chomp($mmin_lon);

    @c = split( /[\t ]/, $br);
    my $mmin_lat = $c[0];
    my $mmax_lon = $c[1];
    chomp($mmax_lon);

    return ($mmin_lat, $mmax_lat, $mmin_lon, $mmax_lon);
}

#===============================================================================

sub qrts2xy {
    my ($string) = @_;

    $xx = 0.0;
    $yy = 0.0;
    $delta = .5;

    $n = length($string);
#    print "$n characters\n";
    for ($i=2;$i<=$n;$i++) {
	$c = substr($string,$i,1);
	if ( $c eq "q" ) {
	    $xx += 0.0;
	    $yy += 0.0;
	} 
	if ( $c eq "r" ) {
	    $xx += $delta;
	    $yy += 0.0;
	}
	if ( $c eq "t" ) {
	    $xx += 0.0;
	    $yy += $delta;
	}
	if ( $c eq "s" ) {
	    $xx += $delta;
	    $yy += $delta;
	}
	$delta /= 2.0;
    }

    $xx /= 2.0;
    $yy /= 2.0;

    return ($xx,$yy);
}

#===============================================================================
	    
sub latlon2qrts {
   
    my ($lat,$lon,$sat_zoom) = @_;
#function GetQuadtreeAddress(long, lat)
#{
    
    $PI = 3.1415926535897;
    $digits = $sat_zoom; # how many digits precision
#now convert to normalized square coordinates
# use standard equations to map into mercator projection
    $x = (180.0 + $lon) / 360.0;
    $y = -$lat * $PI / 180; # convert to radians
    $y = 0.5 * log((1+sin($y)) / (1 - sin($y)));
    $y *= 1.0/(2 * $PI); # scale factor from radians to normalized
    $y += 0.5; # and make y range from 0 - 1
    $quad = "t"; # google addresses start with t

 #   print "original x,y: $x $y\n";

    @lookup = ( "q", "r", "t", "s" );
    
    #$lookup = "qrts"; # tl tr bl br
    while ($digits--) #(post-decrement)
    {
# make sure we only look at fractional part
	#print("x = $x\n");
	$x -= floor($x);
	#printf("int(x) = $x\n");
	$y -= floor($y);
	$index = ($x >= 0.5 ? 1 : 0) + ($y >= 0.5 ? 2 : 0);
	$str = $lookup[$index];
	#print "index: $index ( $str)\n";
	$quad = $quad . $lookup[$index]; #substr((x >= 0.5 ? 1 : 0) + (y >= 0.5 ? 2 : 0), 1);
	# now descend into that square
	$x *= 2;
	$y *= 2;
    }

    #print "quad: $quad\n";

    $test = log(exp(1));
    #print "test: $test\n";

    return $quad;

}

sub round {
    my($number) = shift;
    return int($number + .5);
}

#===============================================================================

# Tries to download Google's map tile for the specified location/zoom, and saves
# it at the specified filename.  If this function gets a 404 error from Google,
# it will subtitude the desired Google image with a copy of "blank.png".
sub get_google_map_tile {
   my ( $la, $lo, $map_zoom, $filename ) = @_;
   my ($ny,$nx) = toNormalizedCoord( $la, $lo );

   $xx = int(floor($nx*2**(17-$map_zoom)));
   $yy = int(floor($ny*2**(17-$map_zoom)));

   print "Trying to retrieve sat image for x = $xx and y = $yy\n";
   
   $url = "http://mt2.google.com/mt?n=404&v=w2.61&x=$xx&y=$yy&zoom=$map_zoom";
   
   print "$url\n";

   $cmd = "wget \"$url\" -O tmp.png -nv";

   $res = qx\$cmd 2>&1\;
   
   # copy blank tile if map tile not found
   if ( $res =~ "ERROR 404" ) {
         
         $cmd = "mogrify -format jpg -quality 100 blank.png";
         
         qx\$cmd\;

         $cmd = "mv blank.jpg $filename";

         qx\$cmd\;

   } else {
         # else convert the map png into a jpeg
         
         print "map download ok\n";

         $cmd = "mogrify -format jpg -quality 100 tmp.png"; 
         
         qx\$cmd\;
         
         $cmd = "mv tmp.jpg $filename";
         
         qx\$cmd\;
   }
}

#===============================================================================

# Attempts to download the specified sub-image from Google Maps (based on the 
# image's qrts name).  If successful this returns 1 and saves the image into
# a file named $local_filename.  
#
# This returns one of "success", "error-404", or "blacklisted".
sub get_google_aerial_image {
   my ($qrts_string, $local_filename) = @_;
#    $new_image = 1;
   
   $sat_ok = 1;


   # Clean up the tempoary image file...
   $temp_imagefile = "download-temp.jpg";
   qx(rm -f $temp_imagefile);

   $cmd = "wget \"http://kh2.google.com/kh?n=404&v=14&t=$qrts_string\" -O $temp_imagefile -nv";
      
   my $res = qx\$cmd 2>&1\;
      
   print $res;

   if ( $res =~ "ERROR 404" ) {
      return "error-404";
   }

   if ( $res =~ "Forbidden" ) {
      return "blacklisted";
   }

   $cmd = "mv $temp_imagefile $local_filename";
   my $res = qx\$cmd 2>&1\;
   return "success";
}

#===============================================================================

# Given a latitude and longitude, this returns the local filename we'll use for
# the corresponding subimage file.
sub compute_subimage_filename {
   my ( $la, $lo, $sat_zoom ) = @_;

#    print "##### >>> la, lo = $la, $lo\n";

   $qrts_string = &latlon2qrts($la, $lo, $sat_zoom);

   ($xx,$yy) = &qrts2xy( $qrts_string );

   $br_x = $xx+$dt;
   $br_y = $yy+$dt;

   ($tl_lat,$tl_lon) = &xy2latlon( $xx, $yy );

   ($br_lat,$br_lon) = &xy2latlon( $br_x, $br_y );

   $t1 = floor($tl_lat);
   $t2 = floor(1000000*($tl_lat-floor($tl_lat)));
   $t3 = floor(-$tl_lon);
   $t4 = floor(1000000*(-$tl_lon-floor(-$tl_lon)));

   $t5 = floor($br_lat);
   $t6 = floor(1000000*($br_lat-floor($br_lat)));
   $t7 = floor(-$br_lon);
   $t8 = floor(1000000*(-$br_lon-floor(-$br_lon)));

   $filename = sprintf("%d.%06d.%d.%06d.%d.%06d.%d.%06d.jpg", 
            $t1,$t2,$t3,$t4,$t5,$t6,$t7,$t8);

#    print "##### <<< compute_subimage_filename: filename = $filename\n\n";

   return $filename
}