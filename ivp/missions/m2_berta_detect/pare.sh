#!/bin/bash 

PARE_WINDOW=15
MARK_LIST="ENCOUNTER"
PARE_LIST="BHV_IPF,VIEW_SEGLIST"
HIT_LIST="*ITER_GAP,*ITER_LEN,PSHARE*,NODE_REPORT*,DB_QOS"

for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	printf "%s [SWITCHES] [time_warp]   \n" $0
	printf "  --just_make, -j    \n" 
	printf "  --help, -h         \n" 
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        PARE_WINDOW=$ARGI
    elif [ "${ARGI}" = "v1" ] ; then
	VARIATION="1"
    else 
	printf "Bad Argument: %s \n" $ARGI
	exit 0
    fi
done

echo "=========================================================ABE"
cd LOG_A*; 
alogscan --rate_only LOG*.alog
alogpare LOG*.alog abe_pared.alog --pare_window=$PARE_WINDOW \
    --markvars=$MARK_LIST --hitvars=$HIT_LIST --parevars=$PARE_LIST
alogscan --rate_only abe_pared.alog
cd ..

echo "=========================================================BEN"
cd LOG_B*; 
alogscan --rate_only LOG*.alog
alogpare LOG*.alog ben_pared.alog --pare_window=$PARE_WINDOW \
    --markvars=$MARK_LIST --hitvars=$HIT_LIST --parevars=$PARE_LIST
alogscan --rate_only ben_pared.alog
cd ..

echo "=========================================================CAL"
cd LOG_C*; 
alogscan --rate_only LOG*.alog
alogpare LOG*.alog cal_pared.alog --pare_window=$PARE_WINDOW \
    --markvars=$MARK_LIST --hitvars=$HIT_LIST --parevars=$PARE_LIST
alogscan --rate_only cal_pared.alog
cd ..

echo "=========================================================DEB"
cd LOG_D*; 
alogscan --rate_only LOG*.alog
alogpare LOG*.alog deb_pared.alog --pare_window=$PARE_WINDOW \
    --markvars=$MARK_LIST --hitvars=$HIT_LIST --parevars=$PARE_LIST
alogscan --rate_only deb_pared.alog
cd ..

echo "============================================ DONE ========="
