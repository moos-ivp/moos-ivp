#/bin/bash
##############################################################################
# AUTHOR:      Christopher Gagner
# DATE:        November 1, 2010
# FILE:        wacoma_nightly_build.sh
# DESCRIPTION: Performs a nightly build of MOOS-IvP on wacoma. Additionally,
#              it performs a nightly build on the virtual machines located
#              on wacoma.
##############################################################################

WORKING_DIR=/Users/gagnercw/repos/moos-ivp-scripts
LOGFILE=${WORKING_DIR}/wacoma_nightly_build.log
LOGCMD="1>> ${LOGFILE} 2>> ${LOGFILE}"
VMRUN="/Library/Application Support/VMware Fusion/vmrun"

#============================================================================
# FUNCTION:    virtual_machine_build
# DESCRIPTION: Start a virtual machine, kicks of a build of MOOS-IvP, and
#              shuts down the virtual machine after the build has been 
#              completed. The name of the VMX file for the virtual machine
#              should be passed in as an argument.
# USAGE:
#    virtual_machine_build /Users/tester/vms/Fedora/Fedora.vmx
#============================================================================
function virtual_machine_build {
   if [ "$1" = "" ] ; then
      echo "ERROR! You must specify a VMX file for 'virtual_machine_build' "
      return -1
   fi
   VM=$1
   if [ ! -e ${VM} ] ; then 
      echo "ERROR! The virtual machine you specified does not exists!"
      return -1
   fi
   # Start the Virtual Machine
   # NOTE: We MUST wrap ${VMRUN} in quotes because there are spaces in the path
   "${VMRUN}" start ${VM} nogui ${LOGCMD}

   # Wait 3 minutes for the Virtual Machine to startup
   sleep 180

   # Invoke the CTest script on the Virtual Machine
   # NOTE: We MUST wrap ${VMRUN} in quotes because there are spaces in the path
   "${VMRUN}" -gu tester -gp moosivptester runScriptInGuest ${VM} \
      /bin/bash "cd /home/tester/repos/moos-ivp-scripts && /usr/bin/ctest -S \
      nightly.cmake -V"

   # Wait 3 minutes before shutting down the Virtual Machine
   sleep 180

   # Stop the Virtual Machine
   # NOTE: We MUST wrap ${VMRUN} in quotes because there are spaces in the path
   "${VMRUN}" stop ${VM} ${LOGCMD}
      

} # END of virtual_machine_build


#=============================================================================
# Move the old log file and create a new log
#=============================================================================
if [ -e ${LOGFILE} ] ; then
   mv ${LOGFILE} ${LOGFILE}.bak
fi
touch ${LOGFILE}
echo "Started MOOS-IvP automated build on `date`" > ${LOGFILE}

#=============================================================================
# Build MOOS-IvP on the Mac
#=============================================================================
# Change into the moos-ivp-scripts directory
cd ${WORKING_DIR}
# Invoke the CTest script
/opt/local/bin/ctest -S nightly.cmake -V ${LOGCMD}


#=============================================================================
# Build MOOS-IvP on the Virtual Machines
#=============================================================================

# Build on Ubuntu 10.10
virtual_machine_build \
   /Users/gagnercw/vms/Ubuntu_10.10_i386.vmwarevm/Ubuntu_10.10_i386.vmx

# Build on Fedora 13
virtual_machine_build \
   /Users/gagnercw/vms/Fedora-13-32bit.vmwarevm/Fedora-13-32bit.vmx


##############################################################################
#                         END of mac_nightly_build.sh
##############################################################################

