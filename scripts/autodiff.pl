#!/usr/bin/perl
# usage: autodiff.pl  dir=dirname *.h *.cpp

use Cwd;

$help       = "";
$otherdir   = "";
#$otherdir   = "project-code-release";
$mode       = "report";

# possible modes:
#   "report"              -r    Default
#   "report-interactive"  -ri
#   "verbose"             -v
#   "terse"               -t

foreach $arg (@ARGV) {
    if((index($arg, "-help") == 0) or
       (index($arg, "--help")== 0) or
       (index($arg, "-h") == 0)){
        $help = "yes";
    }
    if(index($arg, "dir=") == 0) {
        $otherdir = $arg;
        $otherdir =~ s/dir=(.*)/$1/i;
    }
    if((index($arg, "-ri") == 0)) {
        $mode = "report-interactive";
    }
    if((index($arg, "-v") == 0) or
       (index($arg, "--verbose") == 0)) {
        $mode = "verbose";
    }
    if((index($arg, "-t") == 0) or
       (index($arg, "--terse") == 0)) {
        $mode = "terse";
    }
}

#print "viewsize: $size, thumbsize: $thsize, quality: $quality\n";

if($help eq "yes") {
    print "Usage: \n";
    print "  autodiff.pl dir=NAME/OF/OTHER/DIR  \n";
    print "  -r or --report   report mode (default) \n";
    print "  -v or --verbose  verbose mode \n";
    print "  -t or --terse    terse mode \n";
    print "  -ri              report interactive mode \n";
    print "Example: autodiff.pl -ri *.h *.cpp \n";
    exit;
}

if ($otherdir eq "") {
    $thisdir = cwd();
    $trydir  = $thisdir;
#    $trydir =~ s/(.*)project-plusnetN(.*)/$1project-plusnet$2/i;
    $trydir =~ s/(.*)moos-ivp\/trunk\/ivp(.*)/$1moos-ivp-dabob\/ivp$2/i;
    $otherdir = $trydir;
}

opendir(DIR,$otherdir) or die "can't opendir [$otherdir]: $!";
closedir(DIR);

# Count the number of files with the right suffix so we 
# can keep a running progress indicater later on.
$array_size = 0;
foreach $file (@ARGV) {
  next if $file=~/^\.\.?$/; # skip . and ..
  if ($file=~/\.cpp$/i) {
      $array_size++;
  }
  if ($file=~/\.h$/i) {
      $array_size++;
  }
}

#-------------------------------------------------------------------------
if($mode eq "terse") {
    $diff_counter = 0;
    $newfile_counter = 0;
    foreach $file (@ARGV) {
	next if $file=~/^\.\.?$/; # skip . and ..
	if (($file=~/\.cpp$/i) or ($file=~/\.h$/i)) {
	    if (!(-e "$otherdir/$file")) {
		$newfile_counter++;
	    }	    
	    if ((-e "./$file") and (-e "$otherdir/$file")) {
		$result = `diff $file $otherdir/$file \n`;
		if (!($result eq "")) {
		    $diff_counter++;
		}
	    }
	}
    }
    print "$diff_counter of $array_size files with detected differences\n";
    if (!($newfile_counter eq 0)) {
	print "$newfile_counter of $array_size files has no counterpart\n";
    }
}

#------------------------------------------------------------------------
if($mode eq "verbose") {
    $counter = 1;
    foreach $file (@ARGV) {
	next if $file=~/^\.\.?$/; # skip . and ..
	if (($file=~/\.cpp$/i) or ($file=~/\.h$/i)) {
	    if (-e "./$file") {
		print "File $counter of $array_size: $file. Continue? y/n";
		$answer = <STDIN>;
		if($answer eq "n") {
		    print STDOUT "Ignoring $file \n";
		}
		else {
		    $result = `diff $file $otherdir/$file \n`;
		    if (!($result eq "")) {
			print "$result";
			print "$file is different!!!\n";
		    }
		}
		$counter++;
	    }
	}
    }
}

#------------------------------------------------------------------------
if(($mode eq "report") or ($mode eq "report-interactive")) {
    $diff_counter = 0;
    $newfile_counter = 0;
    $diff_files = "";
    $newfile_files = "";
    foreach $file (@ARGV) {
	next if $file=~/^\.\.?$/; # skip . and ..
	if (($file=~/\.cpp$/i) or ($file=~/\.h$/i)) {
	    if (!(-e "$otherdir/$file")) {
		$newfile_counter++;
		$newfile_files = $newfile_files . "   ($newfile_counter) $file\n";
	    }	    
	    if ((-e "./$file") and (-e "$otherdir/$file")) {
		$result = `diff $file $otherdir/$file \n`;
		if (!($result eq "")) {
		    $diff_counter++;
		    $diff_files = $diff_files . "   ($diff_counter) $file\n";
		}
	    }
	}
    }
    print "$diff_counter of $array_size files with detected differences\n";
    if (!($diff_counter eq 0)) {
	print "$diff_files";
    }

    if (!($newfile_counter eq 0)) {
	print "$newfile_counter of $array_size files has no counterpart\n";
	print "$newfile_files";
    }
}



#------------------------------------------------------------------------
if($mode eq "report-interactive") {
    foreach $file (@ARGV) {
	next if $file=~/^\.\.?$/; # skip . and ..
	if (($file=~/\.cpp$/i) or ($file=~/\.h$/i)) {
	    if ((-e "./$file") and (-e "$otherdir/$file")) {
		$result = `diff $file $otherdir/$file \n`;
		if (!($result eq "")) {
		    print "Examine diff for $file? [y]/n";
		    $answer = <STDIN>;
		    if(!($answer eq "n")) {
			print "\n\n\n\n";
			print "$result";
		    }
		}
	    }
	}
    }
}



