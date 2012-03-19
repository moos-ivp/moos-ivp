#!/usr/bin/perl
# usage: mac_network_info.pl

$help       = "";
$verbose    = "";

foreach $arg (@ARGV) {
    if((index($arg, "-help") ==0) or
       (index($arg, "--help")==0) or
       (index($arg, "-h")==0)){
        $help = "yes";
    }
    if((index($arg, "--verbose") ==0) or
       (index($arg, "-v")==0)){
        $verbose = "yes";
    }

}


print "\n\nNetwork Summary:\n";
print "============================================== ";
print "\33[34m";
print "(Airport)\n";
print `networksetup -getinfo Airport`;
print "\33[31m";
print `networksetup -getairportnetwork Airport`;
print "\33[0m";
print "============================================== ";
print "\33[32m";
print "(Ethernet)\n";
print `networksetup -getinfo Ethernet`;
print "\33[0m";

if($verbose eq "yes") {
    print "============================================== ";
    print "\33[36m";
    print "(Ping Info)\n";
    print `ping -o -R 18.38.2.158`;
    print "\33[0m";
    exit;
}

print "============================================== \n";

if($help eq "yes") {
    print "Usage: $0  \n";
    print "  Calls /usr/sbin/networksetup to fetch network info \n";
    exit;
}
