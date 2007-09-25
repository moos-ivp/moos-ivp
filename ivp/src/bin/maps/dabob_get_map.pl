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

$TileDir = "Tiles";


use POSIX;  # to get ceil() and floor()
$lat = 38.872105;
$lon = -77.202770;

print "\nMap Image Generator\n(images from Google)\n\n";
#print " Top left lat-lon (like 38.872105 -77.202770)  --> "; $topleft_latlon = <>;
#print " Bottom right lat-lon --> "; $bottomright_latlon = <>;

# Sample RNDF
#$topleft_latlon = "38.877049 -77.208471";
#$bottomright_latlon = "38.864897 -77.197511";

# Weymouth
#$topleft_latlon = "42.158754 -70.949703";
#$bottomright_latlon = "42.139587 -70.924934";

# Dabob Bay - Washington State
$topleft_latlon = "47.854800 -122.936200";
$bottomright_latlon = "47.654800 -122.736200";

# East Cambridge
#$topleft_latlon = "42.375455 -71.091049";
#$bottomright_latlon = "42.36i2969 -71.076752";

# Cambridge Port
#$topleft_latlon = "42.356225 -71.106077";
#$bottomright_latlon = "42.355704 -71.103537";

($min_lat,$max_lat,$min_lon,$max_lon) = parse_input( $topleft_latlon, $bottomright_latlon );

print "lat: [$min_lat, $max_lat]\n";
print "lon: [$min_lon, $max_lon]\n";

$delta_deg = 360.0 / 2**19;
$delta_deg /= 2.0; # to make sure that we get all images

$dt = 1.0 / 2**19;

#print "delta_deg: $delta_deg\n";

$nlat = floor(($max_lat - $min_lat)/$delta_deg) + 1;
$nlon = floor(($max_lon - $min_lon)/$delta_deg) + 1;
$ntiles = $nlat * $nlon;

print "Number of image tiles: $nlat x $nlon = $ntiles\n";
$count = 0;

for ($la = $min_lat;$la <= $max_lat;$la += $delta_deg) {
    for ($lo = $min_lon; $lo < $max_lon; $lo += $delta_deg) {
	
	print "lat-lon: $la $lo\n";

	$qrts_string = &latlon2qrts($la, $lo);

	#print "solution: $qrts_string\n";
	
	($xx,$yy) = &qrts2xy( $qrts_string );

	$br_x = $xx+$dt;
	$br_y = $yy+$dt;

	($tl_lat,$tl_lon) = &xy2latlon( $xx, $yy );
	

	($br_lat,$br_lon) = &xy2latlon( $br_x, $br_y );
	#$qrts_string2 = &latlon2qrts($la, $lo);

	#print "\n";
	#print "tl lat-lon: $tl_lat $tl_lon\n";
	#print "br lat-lon: $br_lat $br_lon\n";

	$t1 = floor($tl_lat);
	$t2 = floor(1000000*($tl_lat-floor($tl_lat)));
	$t3 = floor(-$tl_lon);
	$t4 = floor(1000000*(-$tl_lon-floor(-$tl_lon)));

	$t5 = floor($br_lat);
	$t6 = floor(1000000*($br_lat-floor($br_lat)));
	$t7 = floor(-$br_lon);
	$t8 = floor(1000000*(-$br_lon-floor(-$br_lon)));

	$filename = "$t1.$t2.$t3.$t4.$t5.$t6.$t7.$t8.jpg";

	if ( ! -e $filename ) {
	    #$cmd = "wget \"http://kh3.google.com/kh?n=404&v=14&t=$qrts_string\" -O $filename -nv";
	    $cmd = "wget \"http://kh2.google.com/kh?n=404&v=14&t=$qrts_string\" -O $filename -nv";
	    #print "cmd: $cmd\n";
	    qx\$cmd\;
        $count++;
		if ( $count > 3 )
		{last;}
	}

        #$qrts_string = "tqstqrqqstttsrtqrsqs";
    }
}


#print "bottom left x,y: $xx $yy\n";



#print "bottom left lat,lon: $bt_lat $bt_lon\n";


exit;



sub xy2latlon {
    my ($x,$y) = @_;
    my $PI = 3.1415926535897;

    my $llon = $x * 360.0 - 180.0;
    $q = exp(4*$PI*($y-.5));
    $a = asin(($q-1)/($q+1));
    my $llat = -180.0*$a/$PI;
    return ($llat,$llon);
}

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
	    
sub latlon2qrts {
   
    my ($lat,$lon) = @_;
#function GetQuadtreeAddress(long, lat)
#{
    
    $PI = 3.1415926535897;
    $digits = 19; # how many digits precision
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

