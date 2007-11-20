#!/usr/bin/python
import sys
import os.path
import datetime
from slog_util import parse_slog

#===============================================================================

def print_usage_and_exit():
   sys.exit("Usage: slog-to-taml.py  <slog-filename>  <taml-filename>")

#===============================================================================

def convert_time(slog_time_str):
   """ Converts a time string from an slog file into a string DTG string for
   use in TAML.  Have to artificially turn (seconds) into (date), so there's 
   some creativity in the conversion.
   """
   
   base_time = datetime.datetime(2007, 1, 1)
   delta = datetime.timedelta(0, float(slog_time_str))
   
   timestamp = base_time + delta
   taml_dtg = timestamp.strftime('%Y-%m-%dT%H:%M:%S')
   return taml_dtg

#===============================================================================

def emit_file_header(taml_file):
   print >> taml_file, \
"""   
<TAML xmlns="urn:us:gov:dod:don:navy:navsea:usw:2:0">
      <Operation xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:udt="urn:us:gov:dod:don:enterprise:udt:1:0" xmlns:usw="urn:us:gov:dod:don:navy:navsea:usw:2:0" xmlns:qdt="urn:us:gov:dod:don:navy:navsea:usw:qdt:1:0">
               <Event>
                     <StartTime>2006-09-12T10:38:49</StartTime>
               </Event>
      </Operation>
      <Platform xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:udt="urn:us:gov:dod:don:enterprise:udt:1:0" xmlns:usw="urn:us:gov:dod:don:navy:navsea:usw:2:0" xmlns:qdt="urn:us:gov:dod:don:navy:navsea:usw:qdt:1:0">
               <Name>USVField_1_1</Name>
               <GUID>USVField_1_1</GUID>
               <ShipClass>USV</ShipClass>
               <ShipType>USV</ShipType>
               <HullNumber>hullnum</HullNumber>
               <Track>"""
               
#===============================================================================

def emit_position_report(taml_file, slog_timestamp, lat, lon, heading):
   
   # TODO: Compute DTG...
   dtg = convert_time(slog_timestamp)
   
   print >> taml_file, \
"""                  <TimePosition>
                           <DateTimeGroup>
                                    <DateTime>%s</DateTime>
                           </DateTimeGroup>
                           <Position>
                                    <AbsolutePosition>
                                          <Latitude>%s</Latitude>
                                          <Longitude>%s</Longitude>
                                          <Altitude>0</Altitude>
                                    </AbsolutePosition>
                           </Position>
                           <Orientation>
                                    <Heading>%s</Heading>
                                    <Pitch>0</Pitch>
                                    <Roll>0</Roll>
                           </Orientation>
                           <Speed>0</Speed>
                  </TimePosition>""" % (dtg, str(lat), str(lon), str(heading))

#===============================================================================

def emit_file_footer(taml_file):
   print >> taml_file, \
"""		</Track>
	</Platform>	
</TAML>"""

#===============================================================================

def slog_to_taml(slog_file, taml_file):
   slog_lines = parse_slog(slog_file)
   
   emit_file_header(taml_file)
   
   for l in slog_lines:
      t = l['TIME']
      x = l['NAV_X']
      y = l['NAV_Y']
      h = l['META_HEADING']
      
      if 'NaN' not in [t, x, y, h]:
         emit_position_report(taml_file, t, y, x, h)
   
   emit_file_footer(taml_file)

#===============================================================================

def main(argv):
   if len(argv) != 3:
      print_usage_and_exit()
   
   slog_filename, taml_filename = argv[1:]
   
   if not os.path.isfile(slog_filename):
      sys.exit("The file \"" + slog_filename + "\" doesn't exist or isn't a file.")
      
   if os.path.exists(taml_filename):
      sys.exit("The TAML file \"" + taml_filename + "\" already exists.")
   
   slog_file = open(slog_filename, "r")
   taml_file = open(taml_filename, "w")
   
   slog_to_taml(slog_file, taml_file)

#===============================================================================
   
if __name__ == '__main__':
   main(sys.argv)