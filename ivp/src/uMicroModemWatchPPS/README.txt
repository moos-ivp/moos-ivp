File: 		uMicroModemWatchPPS
Author: 	Alexander Bahr   abahr@mit.edu
Created: 	5 August 2007
Last updated:	5 August 2007

Synopsis:

	uMicroModemPPSWatch check the traffic coming from the modem for a
	"$CAERR,EXTSYNC timeout on" error message which indicates that a 
	modem set to PPS synced transmission has not received a PPS signal
	for at least one second. After a set number of these error messages
	uMicroModemPPSWatch reconfigures the modem to non-PPS synced data
	transmission

moos-file entries:

	modem_prefix = <same as VarNamePrefix in iMicroModem configuration block>
	max_timeout = <number of timeout errors before reconfiguration occurs>

Subscribed variables:

	<modem_prefix>_RAW

Published variables:

	<modem_prefix>_TO_MODEM : this is where the configuration commands get sent

Reports to the terminal:

	- every time a $CAERR occurs (if verbose = true)
	- every time a "EXTSYNC timeout detected" is detected
	- if PPS has been disabled


