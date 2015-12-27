#!/bin/bash 

cd LOG_A*; alogpare LOG*.alog abe_pared.alog --pare_window=15; cd -
cd LOG_B*; alogpare LOG*.alog ben_pared.alog --pare_window=15; cd -
cd LOG_C*; alogpare LOG*.alog cal_pared.alog --pare_window=15; cd -
cd LOG_D*; alogpare LOG*.alog deb_pared.alog --pare_window=15; cd -

