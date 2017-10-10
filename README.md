#======================================================================================
#======================================================================================
# USB_PTT
# This is the design for a USB Push-To-Talk interface for amateur radio.
# This design was intended with work with the Yaesu FT-8900R, but will also work on radios with a similar interface.
#
# Author: Dylon Mutz, KK6OTK
# Email : dylon.mutz@gmail.com
#
# 10/7/2017 -- Initial release
#======================================================================================
#======================================================================================

99-usb_ptt.rules
   A udev rule is needed to set the device file to the dialout group with read/write permissions.
   Add this rules file to /etc/udev/rules.d/

ptt_driver/ptt_driver.ko
   This is the Linux kernel module (or driver) that is compiled from ptt_driver.c. This is the system-level middleman between the userspace application and the actual USB device hardware.
   The module can be loaded manually running the command "insmod <path to ptt_driver.ko"
   To allow the module to load automatically when the system is booted, follow the next steps:
      1) Make a new module directory in the kernel module folder
            "mkdir /lib/modules/`uname -r`/kernel/driver/ptt_driver"
      2) Allow the modprobe command to find the module by placing it under the "/lib/modules/`uname -r`" directory
            "cp usb_ptt/ptt_driver.ko /lib/modules/`uname -r`/kernel/driver/ptt_driver/"
      3) Regenerate module dependencies
            "depmod -ae
      4) Allow module to automatically load by placing a module config file in the "/etc/modules-load.d" directory
            "cp ptt_driver.conf /etc/modules-load.d/"
