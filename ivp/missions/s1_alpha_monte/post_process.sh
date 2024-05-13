#!/bin/bash -e
#----------------------------------------------------------
#  Script: post_process.sh
#  Author: Michael Benjamin
#  LastEd: May 12th 2024
#----------------------------------------------------------
#  Part 1: Set global var defaults
#----------------------------------------------------------
DF=".LastOpenedMOOSLogDirectory"
cdkk() { if [ -f $DF ]; then DIR=`cat $DF | cut -d '=' -f2`; cd $DIR; fi }

MRM="./"
if [ -f $DF ]; then
    MRM=`cat $DF | cut -d '=' -f2`;
fi 



#cdkk
aloggrep SUMMARY $MRM/*.alog --format=val --subpat=speed:time >> results.txt

#./clean.sh
